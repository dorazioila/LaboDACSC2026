package be.hepl.app.client.encoder;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class ClientBookEncoderFrame extends JFrame {
    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;

    private JComboBox<String> comboAuthors;
    private JComboBox<String> comboSubjects;
    private DefaultTableModel tableModel;

    private JTextField txtTitle, txtIsbn, txtPages, txtYear, txtPrice, txtStock;
    private JButton btnLogin, btnLogout, btnAddAuthor, btnAddSubject, btnAddBook;

    public ClientBookEncoderFrame() {
        super("Client Encodage Livre (Java Swing)");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(900, 600);
        setLocationRelativeTo(null);
        initUI();
    }

    private void initUI() {
        JPanel mainPanel = new JPanel(new BorderLayout());

        // --- Barre de menu / Connexion ---
        JToolBar toolBar = new JToolBar();
        btnLogin = new JButton("Connexion");
        btnLogout = new JButton("Déconnexion");
        btnLogout.setEnabled(false);

        btnLogin.addActionListener(e -> login());
        btnLogout.addActionListener(e -> logout());

        toolBar.add(btnLogin);
        toolBar.add(btnLogout);
        mainPanel.add(toolBar, BorderLayout.NORTH);

        // --- Table des livres ---
        String[] columns = {"ID", "Titre", "Auteur", "Sujet", "ISBN", "Pages", "Année", "Prix", "Stock"};
        tableModel = new DefaultTableModel(columns, 0);
        JTable tableBooks = new JTable(tableModel);
        mainPanel.add(new JScrollPane(tableBooks), BorderLayout.CENTER);

        // --- Formulaire d'ajout ---
        // --- Formulaire d'ajout propre avec GridBagLayout ---
        JPanel formPanel = new JPanel(new GridBagLayout());
        formPanel.setBorder(BorderFactory.createTitledBorder("Encodage Livre"));
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        comboAuthors = new JComboBox<>();
        comboSubjects = new JComboBox<>();

        txtTitle = new JTextField();
        txtIsbn = new JTextField();
        txtPages = new JTextField("0");
        txtYear = new JTextField("2024");
        txtPrice = new JTextField("0.00");
        txtStock = new JTextField("0");

        btnAddAuthor = new JButton("+ Auteur");
        btnAddSubject = new JButton("+ Sujet");
        btnAddBook = new JButton("Ajouter Livre");

        setFormEnabled(false);

        // Ligne 0 : Titre (occupant 3 colonnes)
        gbc.gridx = 0; gbc.gridy = 0; gbc.weightx = 0;
        formPanel.add(new JLabel("Titre:"), gbc);
        gbc.gridx = 1; gbc.gridwidth = 3; gbc.weightx = 1.0;
        formPanel.add(txtTitle, gbc);

        // Ligne 1 : Auteur et Sujet
        gbc.gridwidth = 1; gbc.weightx = 0;
        gbc.gridx = 0; gbc.gridy = 1;
        formPanel.add(new JLabel("Auteur:"), gbc);
        
        JPanel pAuthor = new JPanel(new BorderLayout(5, 0));
        pAuthor.add(comboAuthors, BorderLayout.CENTER);
        pAuthor.add(btnAddAuthor, BorderLayout.EAST);
        gbc.gridx = 1; gbc.weightx = 0.5;
        formPanel.add(pAuthor, gbc);

        gbc.gridx = 2; gbc.weightx = 0;
        formPanel.add(new JLabel("Sujet:"), gbc);

        JPanel pSubject = new JPanel(new BorderLayout(5, 0));
        pSubject.add(comboSubjects, BorderLayout.CENTER);
        pSubject.add(btnAddSubject, BorderLayout.EAST);
        gbc.gridx = 3; gbc.weightx = 0.5;
        formPanel.add(pSubject, gbc);

        // Ligne 2 : ISBN et Pages
        gbc.gridx = 0; gbc.gridy = 2; gbc.weightx = 0;
        formPanel.add(new JLabel("ISBN:"), gbc);
        gbc.gridx = 1; gbc.weightx = 0.5;
        formPanel.add(txtIsbn, gbc);

        gbc.gridx = 2; gbc.weightx = 0;
        formPanel.add(new JLabel("Pages:"), gbc);
        gbc.gridx = 3; gbc.weightx = 0.5;
        formPanel.add(txtPages, gbc);

        // Ligne 3 : Année et Prix
        gbc.gridx = 0; gbc.gridy = 3; gbc.weightx = 0;
        formPanel.add(new JLabel("Année:"), gbc);
        gbc.gridx = 1; gbc.weightx = 0.5;
        formPanel.add(txtYear, gbc);

        gbc.gridx = 2; gbc.weightx = 0;
        formPanel.add(new JLabel("Prix:"), gbc);
        gbc.gridx = 3; gbc.weightx = 0.5;
        formPanel.add(txtPrice, gbc);

        // Ligne 4 : Stock et Bouton Ajouter
        gbc.gridx = 0; gbc.gridy = 4; gbc.weightx = 0;
        formPanel.add(new JLabel("Stock:"), gbc);
        gbc.gridx = 1; gbc.weightx = 0.5;
        formPanel.add(txtStock, gbc);

        gbc.gridx = 2; gbc.gridy = 4; gbc.gridwidth = 2; gbc.weightx = 0.5;
        formPanel.add(btnAddBook, gbc);

        mainPanel.add(formPanel, BorderLayout.SOUTH);

        // Evénements boutons
        btnAddAuthor.addActionListener(e -> addAuthor());
        btnAddSubject.addActionListener(e -> addSubject());
        btnAddBook.addActionListener(e -> addBook());

        add(mainPanel);
    }

    private void setFormEnabled(boolean enabled) {
        btnAddAuthor.setEnabled(enabled);
        btnAddSubject.setEnabled(enabled);
        btnAddBook.setEnabled(enabled);
        comboAuthors.setEnabled(enabled);
        comboSubjects.setEnabled(enabled);
    }

    private void login() {
        String user = JOptionPane.showInputDialog(this, "Utilisateur :");
        String pass = JOptionPane.showInputDialog(this, "Mot de passe :");

        if (user == null || pass == null || user.isEmpty() || pass.isEmpty()) return;

        try {
            if (socket == null || socket.isClosed()) {
                socket = new Socket("127.0.0.1", 50000);
                in = new BufferedReader(new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8));
                out = new PrintWriter(new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8), true);
            }

            out.print("LOGIN#" + user + "#" + pass + "\n");
            out.flush();

            String response = in.readLine();
            if (response != null && response.startsWith("LOGIN#OK")) {
                JOptionPane.showMessageDialog(this, "Connexion réussie !");
                btnLogin.setEnabled(false);
                btnLogout.setEnabled(true);
                setFormEnabled(true);
                loadAuthors();
                loadSubjects();
            } else {
                JOptionPane.showMessageDialog(this, "Échec d'authentification.", "Erreur", JOptionPane.ERROR_MESSAGE);
            }
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this, "Erreur réseau : " + ex.getMessage());
        }
    }

    private void loadAuthors() {
        try {
            comboAuthors.removeAllItems();
            out.print("GET_AUTHORS\n");
            out.flush();

            String response = in.readLine();
            if (response != null && response.startsWith("GET_AUTHORS#OK")) {
                String[] tokens = response.split("#");
                int count = Integer.parseInt(tokens[2]);
                int idx = 3;
                for (int i = 0; i < count; i++) {
                    String nom = tokens[idx++];
                    String prenom = tokens[idx++];
                    comboAuthors.addItem(nom + " " + prenom);
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void loadSubjects() {
        try {
            comboSubjects.removeAllItems();
            out.print("GET_SUBJECTS\n");
            out.flush();

            String response = in.readLine();
            if (response != null && response.startsWith("GET_SUBJECTS#OK")) {
                String[] tokens = response.split("#");
                int count = Integer.parseInt(tokens[2]);
                for (int i = 0; i < count; i++) {
                    comboSubjects.addItem(tokens[3 + i]);
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void addAuthor() {
        String nom = JOptionPane.showInputDialog(this, "Nom de l'auteur :");
        String prenom = JOptionPane.showInputDialog(this, "Prénom de l'auteur :");
        if (nom != null && prenom != null) {
            try {
                out.print("ADD_AUTHOR#" + nom + "#" + prenom + "\n");
                out.flush();
                String res = in.readLine();
                if (res != null && res.startsWith("ADD_AUTHOR#OK")) {
                    comboAuthors.addItem(nom + " " + prenom);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void addSubject() {
        String subject = JOptionPane.showInputDialog(this, "Nom du sujet :");
        if (subject != null) {
            try {
                out.print("ADD_SUBJECT#" + subject + "\n");
                out.flush();
                String res = in.readLine();
                if (res != null && res.startsWith("ADD_SUBJECT#OK")) {
                    comboSubjects.addItem(subject);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void addBook() {
        try {
            String title = txtTitle.getText();
            String author = (String) comboAuthors.getSelectedItem();
            String subject = (String) comboSubjects.getSelectedItem();
            String isbn = txtIsbn.getText();
            String pages = txtPages.getText();
            String year = txtYear.getText();
            String price = txtPrice.getText().replace(',', '.');
            String stock = txtStock.getText();

            String trame = String.format("ADD_BOOK#%s#%s#%s#%s#%s#%s#%s#%s\n",
                    title, author, subject, isbn, pages, year, price, stock);

            out.print(trame);
            out.flush();

            String res = in.readLine();
            if (res != null && res.startsWith("ADD_BOOK#OK")) {
                String[] tokens = res.split("#");
                int newId = Integer.parseInt(tokens[2]);
                tableModel.addRow(new Object[]{newId, title, author, subject, isbn, pages, year, price, stock});
                JOptionPane.showMessageDialog(this, "Livre ajouté !");
            } else {
                JOptionPane.showMessageDialog(this, "Échec d'ajout du livre.");
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void logout() {
        try {
            if (out != null) {
                out.print("LOGOUT\n");
                out.flush();
                socket.close();
            }
        } catch (Exception ignored) {}
        btnLogin.setEnabled(true);
        btnLogout.setEnabled(false);
        setFormEnabled(false);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new ClientBookEncoderFrame().setVisible(true));
    }
}
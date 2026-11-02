package be.hepl.app.client.achat;

import be.hepl.app.entity.Book;
import be.hepl.app.protocol.RequestBSPP;
import be.hepl.app.protocol.ResponseBSPP;
import be.hepl.app.search.BookSearchVM;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.List;

public class ClientAchatFrame extends JFrame {
    private Socket socket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    private int currentClientId = -1;

    private JTextField txtLastName, txtFirstName;
    private JTextField txtSearchTitle, txtMaxPrice;
    private JSpinner spinQuantity;
    private DefaultTableModel modelBooks, modelCaddy;
    private JLabel lblStatus, lblTotal;
    private float totalAmount = 0.0f;

    public ClientAchatFrame() {
        super("Client Achat Magasin (Java Swing - BSPP)");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1000, 700);
        setLocationRelativeTo(null);
        initUI();
        connectServer();
    }

    private void connectServer() {
        try {
            socket = new Socket("127.0.0.1", 50001);
            out = new ObjectOutputStream(socket.getOutputStream());
            out.flush();
            in = new ObjectInputStream(socket.getInputStream());
            lblStatus.setText("Connecté au Serveur Achat (Port 50001)");
        } catch (Exception e) {
            lblStatus.setText("Erreur de connexion au serveur !");
        }
    }

    private void initUI() {
        JPanel mainPanel = new JPanel(new BorderLayout(10, 10));

        // Top : Identification Client
        JPanel pClient = new JPanel(new FlowLayout(FlowLayout.LEFT));
        pClient.setBorder(BorderFactory.createTitledBorder("Identification Client"));
        txtLastName = new JTextField(12);
        txtFirstName = new JTextField(12);
        JButton btnSetClient = new JButton("Valider Client");

        pClient.add(new JLabel("Nom:")); pClient.add(txtLastName);
        pClient.add(new JLabel("Prénom:")); pClient.add(txtFirstName);
        pClient.add(btnSetClient);
        mainPanel.add(pClient, BorderLayout.NORTH);

        // Center : Recherche + Table des livres
        JPanel pCenter = new JPanel(new BorderLayout(5, 5));
        pCenter.setBorder(BorderFactory.createTitledBorder("Livres disponibles"));

        // Utilisation de GridBagLayout pour forcer l'affichage de tous les éléments sans masquage
        JPanel pSearch = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(2, 4, 2, 4);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        txtSearchTitle = new JTextField(8);
        txtMaxPrice = new JTextField(4);
        JButton btnSearch = new JButton("Rechercher");
        spinQuantity = new JSpinner(new SpinnerNumberModel(1, 1, 99, 1));
        JButton btnAddCaddy = new JButton("+ Ajouter au panier");

        gbc.gridx = 0; gbc.gridy = 0; gbc.weightx = 0;
        pSearch.add(new JLabel("Titre:"), gbc);
        gbc.gridx = 1; gbc.weightx = 0.3;
        pSearch.add(txtSearchTitle, gbc);

        gbc.gridx = 2; gbc.weightx = 0;
        pSearch.add(new JLabel("Prix max:"), gbc);
        gbc.gridx = 3; gbc.weightx = 0.2;
        pSearch.add(txtMaxPrice, gbc);

        gbc.gridx = 4; gbc.weightx = 0;
        pSearch.add(btnSearch, gbc);

        gbc.gridx = 5;
        pSearch.add(new JLabel("Qté:"), gbc);
        gbc.gridx = 6;
        pSearch.add(spinQuantity, gbc);

        gbc.gridx = 7; gbc.weightx = 0.5;
        pSearch.add(btnAddCaddy, gbc);

        pCenter.add(pSearch, BorderLayout.NORTH);

        String[] colBooks = {"ID", "Titre", "Auteur", "Sujet", "Prix", "Stock"};
        modelBooks = new DefaultTableModel(colBooks, 0);
        JTable tableBooks = new JTable(modelBooks);
        pCenter.add(new JScrollPane(tableBooks), BorderLayout.CENTER);
        mainPanel.add(pCenter, BorderLayout.CENTER);

        // Right : Panier + Actions (Payer / Annuler)
        JPanel pRight = new JPanel(new BorderLayout(5, 5));
        pRight.setPreferredSize(new Dimension(350, 0));
        pRight.setBorder(BorderFactory.createTitledBorder("Panier en cours"));

        String[] colCaddy = {"Titre", "Qté", "Prix U.", "Total"};
        modelCaddy = new DefaultTableModel(colCaddy, 0);
        JTable tableCaddy = new JTable(modelCaddy);
        pRight.add(new JScrollPane(tableCaddy), BorderLayout.CENTER);

        JPanel pCaddySouth = new JPanel(new GridLayout(3, 1, 5, 5));
        lblTotal = new JLabel("Total : 0.00 €", SwingConstants.CENTER);
        lblTotal.setFont(new Font("SansSerif", Font.BOLD, 16));
        JButton btnPay = new JButton("Payer le panier");
        JButton btnCancel = new JButton("Annuler panier");

        pCaddySouth.add(lblTotal);
        pCaddySouth.add(btnPay);
        pCaddySouth.add(btnCancel);
        pRight.add(pCaddySouth, BorderLayout.SOUTH);

        mainPanel.add(pRight, BorderLayout.EAST);

        // Bottom : Status
        lblStatus = new JLabel("Prêt");
        mainPanel.add(lblStatus, BorderLayout.SOUTH);

        // Evénements
        btnSetClient.addActionListener(e -> setClient());
        btnSearch.addActionListener(e -> searchBooks());
        btnAddCaddy.addActionListener(e -> {
            int row = tableBooks.getSelectedRow();
            if (row != -1 && currentClientId != -1) {
                int bookId = (int) modelBooks.getValueAt(row, 0);
                String title = (String) modelBooks.getValueAt(row, 1);
                float price = (float) modelBooks.getValueAt(row, 4);
                int qty = (int) spinQuantity.getValue();
                addToCaddy(bookId, title, price, qty);
            } else {
                JOptionPane.showMessageDialog(this, "Sélectionnez un livre et identifiez le client.");
            }
        });
        btnCancel.addActionListener(e -> cancelCaddy());
        btnPay.addActionListener(e -> payCaddy());

        add(mainPanel);
    }

    private void setClient() {
        try {
            out.writeObject(new RequestBSPP("CLIENT", new String[]{txtLastName.getText(), txtFirstName.getText()}));
            out.flush();
            ResponseBSPP res = (ResponseBSPP) in.readObject();
            if (res.isSuccess()) {
                currentClientId = (int) res.getData();
                lblStatus.setText("Client actif ID : " + currentClientId);
            }
        } catch (Exception ex) { ex.printStackTrace(); }
    }

    private void searchBooks() {
        try {
            BookSearchVM vm = new BookSearchVM();
            if (!txtSearchTitle.getText().isEmpty()) vm.setTitle(txtSearchTitle.getText());
            if (!txtMaxPrice.getText().isEmpty()) vm.setMaxPrice(Float.parseFloat(txtMaxPrice.getText()));

            out.writeObject(new RequestBSPP("SELECT_BOOK", vm));
            out.flush();
            ResponseBSPP res = (ResponseBSPP) in.readObject();
            if (res.isSuccess()) {
                List<Book> books = (List<Book>) res.getData();
                modelBooks.setRowCount(0);
                for (Book b : books) {
                    modelBooks.addRow(new Object[]{b.getId(), b.getTitle(), b.getAuthorName(), b.getSubjectName(), b.getPrice(), b.getStockQuantity()});
                }
            }
        } catch (Exception ex) { ex.printStackTrace(); }
    }

    private void addToCaddy(int bookId, String title, float price, int qty) {
        try {
            out.writeObject(new RequestBSPP("ADD_CADDY_ITEM", new Object[]{currentClientId, bookId, qty}));
            out.flush();
            ResponseBSPP res = (ResponseBSPP) in.readObject();
            if (res.isSuccess()) {
                modelCaddy.addRow(new Object[]{title, qty, price, price * qty});
                totalAmount += price * qty;
                lblTotal.setText(String.format("Total : %.2f €", totalAmount));
                searchBooks(); // Rafraîchir les stocks affichés
            } else {
                JOptionPane.showMessageDialog(this, res.getMessage());
            }
        } catch (Exception ex) { ex.printStackTrace(); }
    }

    private void cancelCaddy() {
        if (currentClientId == -1) return;
        try {
            out.writeObject(new RequestBSPP("CANCEL_CADDY", currentClientId));
            out.flush();
            ResponseBSPP res = (ResponseBSPP) in.readObject();
            if (res.isSuccess()) {
                resetCaddyUI();
                searchBooks();
            }
        } catch (Exception ex) { ex.printStackTrace(); }
    }

    private void payCaddy() {
        if (currentClientId == -1) return;
        try {
            out.writeObject(new RequestBSPP("PAY_CADDY", currentClientId));
            out.flush();
            ResponseBSPP res = (ResponseBSPP) in.readObject();
            if (res.isSuccess()) {
                JOptionPane.showMessageDialog(this, "Paiement effectué avec succès !");
                resetCaddyUI();
                searchBooks();
            }
        } catch (Exception ex) { ex.printStackTrace(); }
    }

    private void resetCaddyUI() {
        modelCaddy.setRowCount(0);
        totalAmount = 0.0f;
        lblTotal.setText("Total : 0.00 €");
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new ClientAchatFrame().setVisible(true));
    }
}
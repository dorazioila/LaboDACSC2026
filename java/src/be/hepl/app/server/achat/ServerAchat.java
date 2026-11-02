package be.hepl.app.server.achat;

import be.hepl.app.dao.BookDAO;
import be.hepl.app.dao.ClientDAO;
import be.hepl.app.entity.Book;
import be.hepl.app.entity.Client;
import be.hepl.app.protocol.RequestBSPP;
import be.hepl.app.protocol.ResponseBSPP;
import be.hepl.app.search.BookSearchVM;
import be.hepl.app.dao.CaddyDAO;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ServerAchat {
    private static final int PORT = 50001;
    private static final int THREAD_POOL_SIZE = 10;

    private final BookDAO bookDAO = new BookDAO();
    private final ClientDAO clientDAO = new ClientDAO();

    public void start() {
        ExecutorService pool = Executors.newFixedThreadPool(THREAD_POOL_SIZE);
        System.out.println("[Serveur Achat] Démarrage sur le port " + PORT + " (Pool size: " + THREAD_POOL_SIZE + ")...");

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("[Serveur Achat] Nouveau client connecté : " + clientSocket.getInetAddress());
                pool.execute(new ClientHandler(clientSocket));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private class ClientHandler implements Runnable {
        private final Socket socket;

        public ClientHandler(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try (
                ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
                ObjectInputStream in = new ObjectInputStream(socket.getInputStream())
            ) {
                out.flush();

                while (!socket.isClosed()) {
                    Object obj = in.readObject();
                    if (!(obj instanceof RequestBSPP)) break;

                    RequestBSPP request = (RequestBSPP) obj;
                    ResponseBSPP response = handleRequest(request);

                    out.writeObject(response);
                    out.flush();
                }
            } catch (EOFException e) {
                System.out.println("[Serveur Achat] Client déconnecté.");
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                try {
                    socket.close();
                } catch (IOException ignored) {}
            }
        }

        private final CaddyDAO caddyDAO = new CaddyDAO();

        private ResponseBSPP handleRequest(RequestBSPP request) {
            switch (request.getType()) {
                case "CLIENT": {
                    String[] info = (String[]) request.getData();
                    Client c = clientDAO.findOrCreate(info[0], info[1]);
                    return c != null ? new ResponseBSPP(true, c.getId()) : new ResponseBSPP(false, "Erreur Client", null);
                }
                case "SELECT_BOOK": {
                    BookSearchVM vm = (BookSearchVM) request.getData();
                    List<Book> books = bookDAO.search(vm);
                    return new ResponseBSPP(true, books);
                }
                case "ADD_CADDY_ITEM": {
                    // Object[] data -> [clientId (int), bookId (int), quantity (int)]
                    Object[] data = (Object[]) request.getData();
                    boolean ok = caddyDAO.addCaddyItem((int) data[0], (int) data[1], (int) data[2]);
                    return new ResponseBSPP(ok, ok ? "Article ajouté !" : "Stock insuffisant.");
                }
                case "CANCEL_CADDY": {
                    int clientId = (int) request.getData();
                    boolean ok = caddyDAO.cancelCaddy(clientId);
                    return new ResponseBSPP(ok, ok ? "Panier annulé" : "Erreur annulation");
                }
                case "PAY_CADDY": {
                    int clientId = (int) request.getData();
                    boolean ok = caddyDAO.payCaddy(clientId);
                    return new ResponseBSPP(ok, ok ? "Paiement réussi !" : "Erreur paiement");
                }
                default:
                    return new ResponseBSPP(false, "Commande invalide", null);
            }
        }
    }

    public static void main(String[] args) {
        new ServerAchat().start();
    }
}
package be.hepl.app;

import be.hepl.app.dao.DBConnection;

public class MainTest {
    public static void main(String[] args) {
        System.out.println("--- Test de Connexion JDBC ---");
        if (DBConnection.getInstance() != null) {
            System.out.println("OK : Connexion à MariaDB réussie !");
        } else {
            System.err.println("KO : Échec de la connexion.");
        }
    }
}
package be.hepl.app.dao;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DBConnection {
    private static Connection connection = null;
    private static final String URL = "jdbc:mysql://localhost:3306/PourStudent?useSSL=false&serverTimezone=UTC";
    private static final String USER = "Student";
    private static final String PASSWORD = "PassStudent1_";

    private DBConnection() {}

    public static synchronized Connection getInstance() {
        if (connection == null) {
            try {
                Class.forName("com.mysql.cj.jdbc.Driver");
                connection = DriverManager.getConnection(URL, USER, PASSWORD);
                System.out.println("[DBConnection] Connexion à la base de données réussie !");
            } catch (ClassNotFoundException e) {
                System.err.println("[DBConnection] Erreur : Driver JDBC introuvable.");
                e.printStackTrace();
            } catch (SQLException e) {
                System.err.println("[DBConnection] Erreur de connexion SQL : " + e.getMessage());
                e.printStackTrace();
            }
        }
        return connection;
    }

    public static synchronized void closeConnection() {
        if (connection != null) {
            try {
                connection.close();
                connection = null;
                System.out.println("[DBConnection] Connexion fermée.");
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }
}
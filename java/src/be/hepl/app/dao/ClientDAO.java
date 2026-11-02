package be.hepl.app.dao;

import be.hepl.app.entity.Client;
import java.sql.*;

public class ClientDAO {
    private final Connection connection;

    public ClientDAO() {
        this.connection = DBConnection.getInstance();
    }

    public synchronized Client findOrCreate(String lastName, String firstName) {
        // 1. Recherche du client existant (utilisation des colonnes nom et prenom)
        String sqlSelect = "SELECT id FROM clients WHERE nom = ? AND prenom = ?";
        try (PreparedStatement pstmt = connection.prepareStatement(sqlSelect)) {
            pstmt.setString(1, lastName);
            pstmt.setString(2, firstName);
            try (ResultSet rs = pstmt.executeQuery()) {
                if (rs.next()) {
                    return new Client(rs.getInt("id"), lastName, firstName);
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        // 2. Si introuvable, création du nouveau client
        String sqlInsert = "INSERT INTO clients (nom, prenom) VALUES (?, ?)";
        try (PreparedStatement pstmt = connection.prepareStatement(sqlInsert, Statement.RETURN_GENERATED_KEYS)) {
            pstmt.setString(1, lastName);
            pstmt.setString(2, firstName);
            if (pstmt.executeUpdate() > 0) {
                try (ResultSet rs = pstmt.getGeneratedKeys()) {
                    if (rs.next()) {
                        return new Client(rs.getInt(1), lastName, firstName);
                    }
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return null;
    }
}
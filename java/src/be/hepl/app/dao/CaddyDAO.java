package be.hepl.app.dao;

import java.sql.*;

public class CaddyDAO {
    private final Connection connection;

    public CaddyDAO() {
        this.connection = DBConnection.getInstance();
    }

    public synchronized int getOrCreateActiveCaddy(int clientId) {
        String selectSql = "SELECT id FROM caddies WHERE clientId = ? AND payed = 0";
        try (PreparedStatement pstmt = connection.prepareStatement(selectSql)) {
            pstmt.setInt(1, clientId);
            try (ResultSet rs = pstmt.executeQuery()) {
                if (rs.next()) return rs.getInt("id");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        String insertSql = "INSERT INTO caddies (clientId, payed) VALUES (?, 0)";
        try (PreparedStatement pstmt = connection.prepareStatement(insertSql, Statement.RETURN_GENERATED_KEYS)) {
            pstmt.setInt(1, clientId);
            if (pstmt.executeUpdate() > 0) {
                try (ResultSet rs = pstmt.getGeneratedKeys()) {
                    if (rs.next()) return rs.getInt(1);
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return -1;
    }

    public synchronized boolean addCaddyItem(int clientId, int bookId, int quantity) {
        try {
            connection.setAutoCommit(false);

            String stockSql = "SELECT stock_quantity FROM books WHERE id = ?";
            int currentStock = 0;
            try (PreparedStatement pstmt = connection.prepareStatement(stockSql)) {
                pstmt.setInt(1, bookId);
                ResultSet rs = pstmt.executeQuery();
                if (rs.next()) {
                    currentStock = rs.getInt("stock_quantity");
                }
            }

            if (currentStock < quantity) {
                connection.rollback();
                connection.setAutoCommit(true);
                return false;
            }

            String updateStock = "UPDATE books SET stock_quantity = stock_quantity - ? WHERE id = ?";
            try (PreparedStatement pstmt = connection.prepareStatement(updateStock)) {
                pstmt.setInt(1, quantity);
                pstmt.setInt(2, bookId);
                pstmt.executeUpdate();
            }

            int caddyId = getOrCreateActiveCaddy(clientId);
            if (caddyId == -1) {
                connection.rollback();
                connection.setAutoCommit(true);
                return false;
            }

            String checkItem = "SELECT id, quantity FROM caddy_items WHERE caddyId = ? AND bookId = ?";
            int existingItemId = -1;
            int existingQty = 0;
            try (PreparedStatement pstmt = connection.prepareStatement(checkItem)) {
                pstmt.setInt(1, caddyId);
                pstmt.setInt(2, bookId);
                ResultSet rs = pstmt.executeQuery();
                if (rs.next()) {
                    existingItemId = rs.getInt("id");
                    existingQty = rs.getInt("quantity");
                }
            }

            if (existingItemId != -1) {
                String updateItem = "UPDATE caddy_items SET quantity = ? WHERE id = ?";
                try (PreparedStatement pstmt = connection.prepareStatement(updateItem)) {
                    pstmt.setInt(1, existingQty + quantity);
                    pstmt.setInt(2, existingItemId);
                    pstmt.executeUpdate();
                }
            } else {
                String insertItem = "INSERT INTO caddy_items (caddyId, bookId, quantity) VALUES (?, ?, ?)";
                try (PreparedStatement pstmt = connection.prepareStatement(insertItem)) {
                    pstmt.setInt(1, caddyId);
                    pstmt.setInt(2, bookId);
                    pstmt.setInt(3, quantity);
                    pstmt.executeUpdate();
                }
            }

            connection.commit();
            connection.setAutoCommit(true);
            return true;
        } catch (SQLException e) {
            try { connection.rollback(); connection.setAutoCommit(true); } catch (SQLException ignored) {}
            e.printStackTrace();
            return false;
        }
    }

    public synchronized boolean cancelCaddy(int clientId) {
        try {
            connection.setAutoCommit(false);
            
            String selectCaddy = "SELECT id FROM caddies WHERE clientId = ? AND payed = 0";
            int caddyId = -1;
            try (PreparedStatement pstmt = connection.prepareStatement(selectCaddy)) {
                pstmt.setInt(1, clientId);
                ResultSet rs = pstmt.executeQuery();
                if (rs.next()) caddyId = rs.getInt("id");
            }

            if (caddyId == -1) {
                connection.setAutoCommit(true);
                return true;
            }

            String selectItems = "SELECT bookId, quantity FROM caddy_items WHERE caddyId = ?";
            try (PreparedStatement pstmt = connection.prepareStatement(selectItems)) {
                pstmt.setInt(1, caddyId);
                ResultSet rs = pstmt.executeQuery();
                while (rs.next()) {
                    String restoreStock = "UPDATE books SET stock_quantity = stock_quantity + ? WHERE id = ?";
                    try (PreparedStatement pUpdate = connection.prepareStatement(restoreStock)) {
                        pUpdate.setInt(1, rs.getInt("quantity"));
                        pUpdate.setInt(2, rs.getInt("bookId"));
                        pUpdate.executeUpdate();
                    }
                }
            }

            String delItems = "DELETE FROM caddy_items WHERE caddyId = ?";
            try (PreparedStatement pstmt = connection.prepareStatement(delItems)) {
                pstmt.setInt(1, caddyId);
                pstmt.executeUpdate();
            }

            String delCaddy = "DELETE FROM caddies WHERE id = ?";
            try (PreparedStatement pstmt = connection.prepareStatement(delCaddy)) {
                pstmt.setInt(1, caddyId);
                pstmt.executeUpdate();
            }

            connection.commit();
            connection.setAutoCommit(true);
            return true;
        } catch (SQLException e) {
            try { connection.rollback(); connection.setAutoCommit(true); } catch (SQLException ignored) {}
            return false;
        }
    }

    public synchronized boolean payCaddy(int clientId) {
        String selectCaddy = "SELECT id FROM caddies WHERE clientId = ? AND payed = 0";
        int caddyId = -1;
        try (PreparedStatement pstmt = connection.prepareStatement(selectCaddy)) {
            pstmt.setInt(1, clientId);
            ResultSet rs = pstmt.executeQuery();
            if (rs.next()) caddyId = rs.getInt("id");
        } catch (SQLException e) { e.printStackTrace(); }

        if (caddyId == -1) return false;

        String sql = "UPDATE caddies SET payed = 1 WHERE id = ?";
        try (PreparedStatement pstmt = connection.prepareStatement(sql)) {
            pstmt.setInt(1, caddyId);
            return pstmt.executeUpdate() > 0;
        } catch (SQLException e) {
            e.printStackTrace();
            return false;
        }
    }
}
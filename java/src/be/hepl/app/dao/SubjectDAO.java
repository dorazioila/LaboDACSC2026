package be.hepl.app.dao;

import be.hepl.app.entity.Subject;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class SubjectDAO {
    private final Connection connection;

    public SubjectDAO() {
        this.connection = DBConnection.getInstance();
    }

    public synchronized List<Subject> findAll() {
        List<Subject> subjects = new ArrayList<>();
        String sql = "SELECT id, name FROM subjects";

        try (Statement stmt = connection.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {

            while (rs.next()) {
                Subject s = new Subject(rs.getInt("id"), rs.getString("name"));
                subjects.add(s);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return subjects;
    }

    public synchronized boolean create(Subject subject) {
        String sql = "INSERT INTO subjects (name) VALUES (?)";

        try (PreparedStatement pstmt = connection.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            pstmt.setString(1, subject.getName());
            int affectedRows = pstmt.executeUpdate();

            if (affectedRows > 0) {
                try (ResultSet generatedKeys = pstmt.getGeneratedKeys()) {
                    if (generatedKeys.next()) {
                        subject.setId(generatedKeys.getInt(1));
                    }
                }
                return true;
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return false;
    }
}
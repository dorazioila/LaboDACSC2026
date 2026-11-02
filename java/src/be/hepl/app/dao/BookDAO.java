package be.hepl.app.dao;

import be.hepl.app.entity.Book;
import be.hepl.app.search.BookSearchVM;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class BookDAO {
    private final Connection connection;

    public BookDAO() {
        this.connection = DBConnection.getInstance();
    }

    public synchronized List<Book> search(BookSearchVM vm) {
        List<Book> books = new ArrayList<>();
        
        StringBuilder sql = new StringBuilder(
            "SELECT b.*, CONCAT(a.last_name, ' ', a.first_name) AS author_name, s.name AS subject_name " +
            "FROM books b " +
            "LEFT JOIN authors a ON b.author_id = a.id " +
            "LEFT JOIN subjects s ON b.subject_id = s.id WHERE 1=1 "
        );

        List<Object> params = new ArrayList<>();

        if (vm != null) {
            if (vm.getId() != null) {
                sql.append("AND b.id = ? ");
                params.add(vm.getId());
            }
            if (vm.getTitle() != null && !vm.getTitle().isEmpty()) {
                sql.append("AND b.title LIKE ? ");
                params.add("%" + vm.getTitle() + "%");
            }
            if (vm.getAuthorLastName() != null && !vm.getAuthorLastName().isEmpty()) {
                sql.append("AND a.last_name LIKE ? ");
                params.add("%" + vm.getAuthorLastName() + "%");
            }
            if (vm.getAuthorFirstName() != null && !vm.getAuthorFirstName().isEmpty()) {
                sql.append("AND a.first_name LIKE ? ");
                params.add("%" + vm.getAuthorFirstName() + "%");
            }
            if (vm.getSubjectName() != null && !vm.getSubjectName().isEmpty()) {
                sql.append("AND s.name LIKE ? ");
                params.add("%" + vm.getSubjectName() + "%");
            }
            if (vm.getMaxPrice() != null) {
                sql.append("AND b.price <= ? ");
                params.add(vm.getMaxPrice());
            }
        }

        try (PreparedStatement pstmt = connection.prepareStatement(sql.toString())) {
            for (int i = 0; i < params.size(); i++) {
                pstmt.setObject(i + 1, params.get(i));
            }

            try (ResultSet rs = pstmt.executeQuery()) {
                while (rs.next()) {
                    Book book = new Book(
                        rs.getInt("id"),
                        rs.getInt("author_id"),
                        rs.getInt("subject_id"),
                        rs.getString("title"),
                        rs.getString("isbn"),
                        rs.getInt("page_count"),
                        rs.getInt("stock_quantity"),
                        rs.getFloat("price"),
                        rs.getInt("publish_year")
                    );
                    book.setAuthorName(rs.getString("author_name"));
                    book.setSubjectName(rs.getString("subject_name"));
                    books.add(book);
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return books;
    }
}
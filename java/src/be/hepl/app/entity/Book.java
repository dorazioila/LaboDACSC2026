package be.hepl.app.entity;

import java.io.Serializable;

public class Book implements Serializable {
    private int id;
    private int authorId;
    private int subjectId;
    private String title;
    private String isbn;
    private int pageCount;
    private int stockQuantity;
    private float price;
    private int publishYear;

    private String authorName;
    private String subjectName;

    public Book() {}

    public Book(int id, int authorId, int subjectId, String title, String isbn, 
                int pageCount, int stockQuantity, float price, int publishYear) {
        this.id = id;
        this.authorId = authorId;
        this.subjectId = subjectId;
        this.title = title;
        this.isbn = isbn;
        this.pageCount = pageCount;
        this.stockQuantity = stockQuantity;
        this.price = price;
        this.publishYear = publishYear;
    }

    public int getId() { return id; }
    public void setId(int id) { this.id = id; }

    public int getAuthorId() { return authorId; }
    public void setAuthorId(int authorId) { this.authorId = authorId; }

    public int getSubjectId() { return subjectId; }
    public void setSubjectId(int subjectId) { this.subjectId = subjectId; }

    public String getTitle() { return title; }
    public void setTitle(String title) { this.title = title; }

    public String getIsbn() { return isbn; }
    public void setIsbn(String isbn) { this.isbn = isbn; }

    public int getPageCount() { return pageCount; }
    public void setPageCount(int pageCount) { this.pageCount = pageCount; }

    public int getStockQuantity() { return stockQuantity; }
    public void setStockQuantity(int stockQuantity) { this.stockQuantity = stockQuantity; }

    public float getPrice() { return price; }
    public void setPrice(float price) { this.price = price; }

    public int getPublishYear() { return publishYear; }
    public void setPublishYear(int publishYear) { this.publishYear = publishYear; }

    public String getAuthorName() { return authorName; }
    public void setAuthorName(String authorName) { this.authorName = authorName; }

    public String getSubjectName() { return subjectName; }
    public void setSubjectName(String subjectName) { this.subjectName = subjectName; }
}
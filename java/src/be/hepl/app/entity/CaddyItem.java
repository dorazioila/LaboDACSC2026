package be.hepl.app.entity;

import java.io.Serializable;

public class CaddyItem implements Serializable {
    private static final long serialVersionUID = 1L;

    private int id;
    private int caddyId;
    private int bookId;
    private String bookTitle;
    private int quantity;
    private float unitPrice;

    public CaddyItem() {}

    public CaddyItem(int bookId, String bookTitle, int quantity, float unitPrice) {
        this.bookId = bookId;
        this.bookTitle = bookTitle;
        this.quantity = quantity;
        this.unitPrice = unitPrice;
    }

    public int getId() { return id; }
    public void setId(int id) { this.id = id; }

    public int getCaddyId() { return caddyId; }
    public void setCaddyId(int caddyId) { this.caddyId = caddyId; }

    public int getBookId() { return bookId; }
    public void setBookId(int bookId) { this.bookId = bookId; }

    public String getBookTitle() { return bookTitle; }
    public void setBookTitle(String bookTitle) { this.bookTitle = bookTitle; }

    public int getQuantity() { return quantity; }
    public void setQuantity(int quantity) { this.quantity = quantity; }

    public float getUnitPrice() { return unitPrice; }
    public void setUnitPrice(float unitPrice) { this.unitPrice = unitPrice; }

    public float getTotalPrice() { return quantity * unitPrice; }
}
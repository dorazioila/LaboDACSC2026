package be.hepl.app.entity;

import java.io.Serializable;

public class Client implements Serializable {
    private int id;
    private String lastName;
    private String firstName;

    public Client() {}

    public Client(int id, String lastName, String firstName) {
        this.id = id;
        this.lastName = lastName;
        this.firstName = firstName;
    }

    public int getId() { 
        return id; 
    }
    public void setId(int id) { 
        this.id = id; 
    }
    public String getLastName() { 
        return lastName; 
    }
    public String getFirstName() { 
        return firstName; 
    }
}
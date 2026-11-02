package be.hepl.app.entity;
import java.io.Serializable;

public class Author implements Serializable{
	private int id;
	private String lastName;
	private String firstName;

	public Author(){
	}
	public Author(int id, String lastName, String firstName){
		this.id = id;
		this.lastName = lastName;
		this.firstName = firstName;
	}

	public int getId() { return id; }
    public void setId(int id) { this.id = id; }

    public String getLastName() { return lastName; }
    public void setLastName(String lastName) { this.lastName = lastName; }

    public String getFirstName() { return firstName; }
    public void setFirstName(String firstName) { this.firstName = firstName; }

    @Override
    public String toString() {
        return lastName + " " + firstName;
    }
}
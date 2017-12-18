package com.shilling.faircall;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Optional;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.google.inject.Singleton;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.guice.FaircallModule;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Sections;

import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.collections.transformation.SortedList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.TextAlignment;

@Singleton
public class Faircall extends Application
{
	
	public static final Injector injector = Guice.createInjector(new FaircallModule());
	
	private final DataContainer data = injector.getInstance(DataContainer.class);
	
    public static void main( String[] args )
    {
    	Application.launch (args);
    }

	@Override
	public void start(Stage stage) throws Exception {
		BorderPane pane = new BorderPane ();
		HBox actionBar = new HBox();
		VBox content = new VBox();
		
		ListView<String> students = new ListView<> (this.data.getObservableStudents());
		pane.setLeft(students);
		pane.setTop (actionBar);
		pane.setCenter(content);
		
		actionBar.setPadding (new Insets (15, 12, 15, 12));
		actionBar.setSpacing(10);
		
		Button addClass = new Button ("Add Class");
		addClass.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Dialog<String> dialog = new TextInputDialog();
				dialog.setTitle("New Class");
				dialog.setHeaderText("Enter class name:");
				
				Optional<String> name = dialog.showAndWait();
				if (name.isPresent() && !name.get().isEmpty()) {
					Faircall.this.data.createClass(name.get());
				}
			}
			
		});
		
		Button addStudent = new Button ("Add Student");
		addStudent.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Dialog<String> dialog = new TextInputDialog();
				dialog.setTitle("New Class");
				dialog.setHeaderText("Enter class name:");
				
				Optional<String> name = dialog.showAndWait();
				if (name.isPresent() && !name.get().isEmpty()) {
					Faircall.this.data.createStudent(name.get());
				}
			}
			
		});
		
		ChoiceBox<String> classes = 
				new ChoiceBox<String> (this.data.getObservableClasses());
		classes.setTooltip(new Tooltip ("Select open class"));
		
		Optional<Section> selected = this.data.getSelected();
		if (selected.isPresent()) {
			classes.getSelectionModel().select(selected.get().getName());
		}
		classes.getSelectionModel().selectedItemProperty().addListener(
				new ChangeListener<String> () {

					@Override
					public void changed(ObservableValue<? extends String> arg0, String old, String selection) {
						Faircall.this.data.select(selection);
					}
					
				});
		
		Button deleteClass = new Button ("Delete Class");
		deleteClass.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Faircall.this.data.deleteClass();
			}
			
		});
		
		Button deleteStudent = new Button ("Delete Student");
		deleteStudent.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				String name = students.getSelectionModel().getSelectedItem();
				Faircall.this.data.deleteStudent(name);
			}
			
		});
		
		Button absent = new Button ("Absent");
		absent.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				String name = students.getSelectionModel().getSelectedItem();
				Faircall.this.data.absentStudent(name);
			}
			
		});
		
		actionBar.getChildren().addAll(addClass, addStudent, classes, absent, deleteClass, deleteStudent);
		
		Label name = new Label ("Hello, World!");
		name.setWrapText(true);
		name.setContentDisplay(ContentDisplay.CENTER);
		name.setTextAlignment(TextAlignment.CENTER);
		
		name.setFont(new Font ("Arial", 40));
		
		content.getChildren().add(name);
		
		stage.setTitle("Faircall");
		stage.setScene (new Scene (pane, 600, 300));
		
		stage.show();
	}
}

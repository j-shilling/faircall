package com.shilling.faircall;

import java.util.Optional;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.google.inject.Singleton;
import com.shilling.faircall.guice.FaircallModule;
import com.shilling.faircall.model.Section;
import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.stage.Stage;
import javafx.util.StringConverter;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;

@Singleton
public class Faircall extends Application
{
	
	public static final Injector injector = Guice.createInjector(new FaircallModule());
	
	private final DataContainer data = injector.getInstance(DataContainer.class);
	private final StudentListView students = injector.getInstance(StudentListView.class);
	private final ContentArea content = injector.getInstance(ContentArea.class);
	
    public static void main( String[] args )
    {
    	Application.launch (args);
    }

	@Override
	public void start(Stage stage) throws Exception {
		BorderPane pane = new BorderPane ();
		HBox actionBar = new HBox();
		
		pane.setLeft(this.students);
		pane.setTop (actionBar);
		pane.setCenter(this.content);
		pane.setCenterShape(true);
		
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
		
		ChoiceBox<Boolean> mode = new ChoiceBox<Boolean> ();
		mode.getItems().addAll(true, false);
		mode.setConverter(new StringConverter<Boolean> () {

			@Override
			public Boolean fromString(String arg0) {
				return "Call randomly".equals(arg0);
			}

			@Override
			public String toString(Boolean arg0) {
				if (arg0)
					return new String ("Call randomly");
				else
					return new String ("Call evenly");
			}
			
		});
		mode.getSelectionModel().select(true);
		mode.getSelectionModel().selectedItemProperty().addListener(
				new ChangeListener<Boolean> () {

					@Override
					public void changed(ObservableValue<? extends Boolean> arg0,
							Boolean old, Boolean cur) {
						Faircall.this.data.setMode(cur);
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
						mode.getSelectionModel().select(Faircall.this.data.getMode());
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
				String name = students.getSelected();
				Faircall.this.data.deleteStudent(name);
			}
			
		});
		
		actionBar.getChildren().addAll(
				addClass, 
				addStudent,
				classes, 
				deleteClass, 
				deleteStudent,
				mode);
		
		stage.setTitle("Faircall");
		stage.setScene (new Scene (pane, 600, 300));
		
		stage.show();
	}
}

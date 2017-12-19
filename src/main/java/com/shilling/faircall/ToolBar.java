package com.shilling.faircall;

import java.util.Optional;

import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Student;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.util.StringConverter;

@Singleton
public class ToolBar extends VBox {

	private final DataContainer data;
	private final StudentListView students;
	
	@Inject
	private ToolBar (DataContainer data, StudentListView students) {
		this.data = data;
		this.students = students;
		
		this.setPadding (new Insets (15, 12, 15, 12));
		this.setSpacing(10);
		
		HBox btns = new HBox();
		btns.setSpacing(10);
		
		Button addClass = new Button ("Add Class");
		addClass.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Dialog<String> dialog = new TextInputDialog();
				dialog.setTitle("New Class");
				dialog.setHeaderText("Enter class name:");
				
				Optional<String> name = dialog.showAndWait();
				if (name.isPresent() && !name.get().isEmpty()) {
					ToolBar.this.data.createClass(name.get());
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
					ToolBar.this.data.createStudent(name.get());
				}
			}
			
		});
		addStudent.setDisable(!this.data.getSelected().isPresent());
		this.data.addSelectionChangedListener(v -> addStudent.setDisable(!v.isPresent()) );
		
		Button deleteClass = new Button ("Delete Class");
		deleteClass.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				ToolBar.this.data.deleteClass();
			}
			
		});
		deleteClass.setDisable(!this.data.getSelected().isPresent());
		this.data.addSelectionChangedListener(v -> deleteClass.setDisable(!v.isPresent()) );
		
		Button deleteStudent = new Button ("Delete Student");
		deleteStudent.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Optional<Student> item = ToolBar.this.students.getSelected();
				if (item.isPresent())
					ToolBar.this.data.deleteStudent(item.get().getName());
			}
			
		});
		deleteStudent.setDisable(!this.students.getSelected().isPresent());
		this.students
			.getSelectionModel()
			.selectedItemProperty()
			.addListener(new ChangeListener<Student> () {

				@Override
				public void changed(
						ObservableValue<? extends Student> observable, 
						Student old, 
						Student cur) {
					
					deleteStudent.setDisable(null == cur);
				}
				
			});
		
		btns.getChildren().addAll(addClass, addStudent, deleteClass, deleteStudent);
		
		HBox choices = new HBox();
		choices.setSpacing(10);
		
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
						ToolBar.this.data.setMode(cur);
					}
					
				});
		mode.setDisable(!this.data.getSelected().isPresent());
		this.data.addSelectionChangedListener(v -> mode.setDisable(!v.isPresent()) );
		
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
						ToolBar.this.data.select(selection);
						mode.getSelectionModel().select(ToolBar.this.data.getMode());
					}
				});
		
		choices.getChildren().addAll(classes, mode);
		
		this.getChildren().addAll(btns, choices);
	}
}

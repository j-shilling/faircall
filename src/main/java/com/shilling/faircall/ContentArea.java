package com.shilling.faircall;

import java.util.Optional;

import com.google.inject.Inject;
import com.shilling.faircall.model.Student;

import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.geometry.VPos;
import javafx.scene.control.Button;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.Label;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.RowConstraints;
import javafx.scene.text.Font;
import javafx.scene.text.TextAlignment;

public class ContentArea extends GridPane {

	private final DataContainer data;
	private final ObservableList<Student> students;
	
	@Inject
	private ContentArea (DataContainer data) {
		this.data = data;
		this.students = this.data.getObservableStudents();
		
		ColumnConstraints col1 = new ColumnConstraints();
		col1.setPercentWidth(25);
		col1.setHalignment(HPos.LEFT);
		ColumnConstraints col2 = new ColumnConstraints();
		col2.setPercentWidth(25);
		col2.setHalignment(HPos.LEFT);
		ColumnConstraints col3 = new ColumnConstraints();
		col3.setPercentWidth(50);
		col3.setHalignment(HPos.RIGHT);
		
		RowConstraints row1 = new RowConstraints();
		row1.setPercentHeight(80);
		row1.setValignment(VPos.CENTER);
		RowConstraints row2 = new RowConstraints();
		row2.setPercentHeight(20);
		row2.setValignment(VPos.CENTER);
		
		this.getColumnConstraints().addAll(col1, col2, col3);
		this.getRowConstraints().addAll(row1, row2);
		
		Label name = new Label (this.data.lastCalled().isPresent() ? this.data.lastCalled().get() : "");
		name.setWrapText(true);
		name.setContentDisplay(ContentDisplay.CENTER);
		name.setTextAlignment(TextAlignment.CENTER);
		
		HBox hbox = new HBox();
		hbox.setAlignment(Pos.CENTER);
		hbox.getChildren().add(name);
		
		name.setFont(new Font ("Arial", 40));
		
		Button absent = new Button ("Absent");
		absent.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Optional<String> name = ContentArea.this.data.lastCalled();
				if (name.isPresent())
					ContentArea.this.data.absentStudent(name.get());
				absent.setDisable(true);
			}
			
		});
		
		Button undo = new Button ("Undo");
		undo.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				ContentArea.this.data.undo();
				name.setText(data.lastCalled().isPresent() ? data.lastCalled().get() : "");
				
				if (!ContentArea.this.data.canUndo())
					undo.setDisable(true);
				absent.setDisable(!ContentArea.this.data.lastCalled().isPresent());
			}
			
		});
		undo.setDisable(true);
		
		Button next = new Button ("Next");
		next.setOnAction(new EventHandler<ActionEvent> () {

			@Override
			public void handle(ActionEvent arg0) {
				Optional<String> result = ContentArea.this.data.callStudent();
				if (result.isPresent())
					name.setText(result.get());
				else
					name.setText("Err");
				
				if (ContentArea.this.data.canUndo())
					undo.setDisable(false);
				absent.setDisable(!ContentArea.this.data.lastCalled().isPresent());
			}
			
		});
		
		this.students.addListener(new ListChangeListener<Student> () {

			@Override
			public void onChanged(Change<? extends Student> change) {
				next.setDisable(ContentArea.this.students.size() < 2);
			}
			
		});
		
		absent.setDisable(!ContentArea.this.data.lastCalled().isPresent());
		
		this.add(hbox, 0, 0, 3, 1);
		this.add(undo, 0, 1);
		this.add(absent, 1, 1);
		this.add(next, 2, 1);
		
		this.setPadding(new Insets (15, 12, 15, 12));
	}
}

package com.shilling.faircall;

import java.util.Optional;

import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Student;

import javafx.scene.control.ListView;

@Singleton
public class StudentListView extends ListView<Student> {

	//private final DataContainer data;
	
	@Inject
	private StudentListView (DataContainer data) {
		super (data.getObservableStudents());
		//this.data = data;
	}
	
	public Optional<Student> getSelected () {
		return Optional.ofNullable(this.getSelectionModel().getSelectedItem());
	}
}

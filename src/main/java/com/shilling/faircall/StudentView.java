package com.shilling.faircall;

import java.util.Comparator;
import java.util.Optional;

import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Student;

import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;

@Singleton
public class StudentView extends TableView<Student> {

	@SuppressWarnings("unchecked")
	@Inject
	private StudentView (DataContainer data) {
		this.setEditable(false);
		
		TableColumn<Student, String> col1 = new TableColumn<> ("Name");
		col1.setCellValueFactory(
				new PropertyValueFactory<Student, String> ("name"));
		col1.setComparator(new Comparator<String> () {

			@Override
			public int compare(String arg0, String arg1) {
				return arg0.compareTo(arg1);
			}
			
		});
		TableColumn<Student, Integer> col2 = new TableColumn<> ("Called");
		col2.setCellValueFactory(
				new PropertyValueFactory<Student, Integer> ("called"));
		col2.setComparator(Integer::compare);
		
		this.getColumns().addAll(col1, col2);
		
		this.setItems(data.getObservableStudents());
	}
	
	public Optional<Student> getSelected () {
		return Optional.ofNullable(this.getSelectionModel().getSelectedItem());
	}
}

package com.shilling.faircall;

import java.util.Comparator;
import java.util.Optional;

import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Student;

import javafx.scene.control.TableColumn;
import javafx.scene.control.TableRow;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Callback;

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
			public int compare(String a, String b) {
				return a.compareTo(b);
			}
			
		});
		TableColumn<Student, Integer> col2 = new TableColumn<> ("Called");
		col2.setCellValueFactory(
				new PropertyValueFactory<Student, Integer> ("called"));
		col2.setComparator(Integer::compare);
		
		this.getColumns().addAll(col1, col2);
		
		this.setRowFactory(new Callback<TableView<Student>, TableRow<Student>> () {

			@Override
			public TableRow<Student> call(TableView<Student> arg0) {
				TableRow<Student> row = new TableRow<>();
				row.setOnMouseClicked(event -> {
					if (event.getClickCount() == 2 && !row.isEmpty())
						data.callStudent(row.getItem());
				});
				
				return row;
			}
			
		});
		
		this.setItems(data.getObservableStudents());
	}
	
	public Optional<Student> getSelected () {
		return Optional.ofNullable(this.getSelectionModel().getSelectedItem());
	}
}

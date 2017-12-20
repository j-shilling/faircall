package com.shilling.faircall;

import java.util.Comparator;
import java.util.Optional;

import com.google.common.collect.ComparisonChain;
import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Student;

import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;

@Singleton
public class StudentView extends TableView<Student> {
	
	private final DataContainer data;

	@SuppressWarnings("unchecked")
	@Inject
	private StudentView (DataContainer data) {
		this.setEditable(false);
		
		this.data = data;
		
		TableColumn<Student, String> col1 = new TableColumn<> ("Name");
		col1.setCellValueFactory(
				new PropertyValueFactory<Student, String> ("name"));
		col1.setComparator(new Comparator<String> () {

			@Override
			public int compare(String a, String b) {
				Optional<Student> x = data.getStudent(a);
				Optional<Student> y = data.getStudent(b);
				
				if (x.isPresent() && y.isPresent())
					return ComparisonChain.start()
							.compare(x.get().getName(), y.get().getName())
							.compare(x.get().getCalled(), y.get().getCalled())
							.result();
				else if (x.isPresent())
					return 1;
				else if (y.isPresent())
					return -1;
				else
					return 0;
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

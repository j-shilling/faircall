package com.shilling.faircall;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Optional;

import com.google.common.base.Preconditions;
import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Sections;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.transformation.SortedList;

@Singleton
public class DataContainer {

	private final Sections sections;
	private final SectionsDAO dao;
	private final ObservableList<String> classes;
	private final ObservableList<String> students;
	
	private final Comparator<String> cmptr = new Comparator<String> () {
		
		@Override
		public int compare(String x, String y) {
			return x.compareTo(y);
		}
		
	};
	
	@Inject
	public DataContainer (Sections sections, SectionsDAO dao) {
		Preconditions.checkNotNull(sections);
		
		this.sections = sections;
		this.dao = dao;
		this.classes = 
			FXCollections.observableList(new ArrayList<String> (sections.getSectionNames()));
		
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent())
			this.students =
				FXCollections.observableList(new ArrayList<String> (cur.get().getStudentNames()));
		else
			this.students =
				FXCollections.observableList(new ArrayList<String> ());
	}
	
	public ObservableList<String> getObservableClasses() {
		return new SortedList<String> (this.classes, this.cmptr);
	}
	
	public ObservableList<String> getObservableStudents() {
		return new SortedList<String> (this.students, this.cmptr);
	}
	
	public void createClass (String name) {
		if (this.sections.addSection(name)) {
			this.classes.add(name);
			this.dao.save(this.sections);
		}
	}
	
	public void deleteClass () {
		Optional<Section> cur = this.getSelected();
		if (cur.isPresent()) {
			if (this.sections.delSection(cur.get().getName())) {
				this.classes.remove(cur.get().getName());
				this.dao.save(this.sections);
			}
		}
		this.unselect();
		
	}
	
	public void unselect () {
		this.sections.unselect();
		this.students.clear();
	}
	
	public void select (String name) {
		if (this.sections.select(name)) {
			this.students.clear();
			this.students.addAll(this.sections.getSelected().get().getStudentNames());
		} else {
			this.unselect();
		}
	}
	
	public void createStudent (String name) {
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent()) {
			if (cur.get().addStudent(name)) {
				this.students.add(name);
				this.dao.save(this.sections);
			}
		}
	}
	
	public void deleteStudent (String name) {
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent()) {
			if (cur.get().delStudent(name)) {
				this.students.remove(name);
				this.dao.save(this.sections);
			}
		}
	}
	
	public void absentStudent (String name) {
		this.students.remove(name);
	}
	
	public Optional<Section> getSelected () {
		return this.sections.getSelected();
	}
	
	public boolean getMode () {
		Optional<Section> selected = this.getSelected();
		return selected.isPresent() ? selected.get().getRandom() : true;
	}
	
	public void setMode (boolean mode) {
		Optional<Section> selected = this.getSelected();
		if (selected.isPresent()) {
			selected.get().setRandom(mode);
			this.dao.save(this.sections);
		}
	}
	
}

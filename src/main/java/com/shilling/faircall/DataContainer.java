package com.shilling.faircall;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.Optional;
import java.util.Stack;

import com.google.common.base.Preconditions;
import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Sections;
import com.shilling.faircall.model.Student;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.transformation.SortedList;

@Singleton
public class DataContainer {
	
	@FunctionalInterface
	public interface ValueChangedListener <T> {
		public void onValueChanged (T val);
	}
	
	private Sections sections;
	private final SectionsDAO dao;
	private final Caller caller;
	private final ObservableList<String> classes;
	private final ObservableList<Student> students;
	
	private final Stack<Sections> history;
	
	private final Collection<ValueChangedListener<Optional<Section>>> selectedListeners;
	
	private final Comparator<String> strcmp = new Comparator<String> () {
		
		@Override
		public int compare(String x, String y) {
			return x.compareTo(y);
		}
		
	};
	
	private final Comparator<Student> stucmp = new Comparator<Student> () {

		@Override
		public int compare(Student x, Student y) {
			return x.compareTo(y);
		}
		
	};
	
	@Inject
	public DataContainer (Sections sections, Caller caller, SectionsDAO dao) {
		Preconditions.checkNotNull(sections);
		
		this.history = new Stack<>();
		this.sections = sections;
		this.caller = caller;
		this.dao = dao;
		this.selectedListeners = new ArrayList<>();
		this.classes = 
			FXCollections.observableList(new ArrayList<String> (sections.getSectionNames()));
		
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent())
			this.students =
				FXCollections.observableList(new ArrayList<Student> (cur.get().getStudents()));
		else
			this.students =
				FXCollections.observableList(new ArrayList<Student> ());
	}
	
	public ObservableList<String> getObservableClasses() {
		return new SortedList<String> (this.classes, this.strcmp);
	}
	
	public ObservableList<Student> getObservableStudents() {
		return new SortedList<Student> (this.students, this.stucmp);
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
		for (ValueChangedListener<Optional<Section>> l : this.selectedListeners)
			l.onValueChanged(this.getSelected());
	}
	
	public void select (String name) {
		if (this.sections.select(name)) {
			this.students.clear();
			this.students.addAll(this.sections.getSelected().get().getStudents());
		} else {
			this.unselect();
		}
		
		for (ValueChangedListener<Optional<Section>> l : this.selectedListeners)
			l.onValueChanged(this.getSelected());
	}
	
	public void createStudent (String name) {
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent()) {
			if (cur.get().addStudent(name)) {
				this.students.add(new Student (name));
				this.dao.save(this.sections);
			}
		}
	}
	
	public void deleteStudent (String name) {
		Optional<Section> cur = this.sections.getSelected();
		if (cur.isPresent()) {
			if (cur.get().delStudent(name)) {
				this.students.remove(new Student (name));
				this.dao.save(this.sections);
			}
		}
	}
	
	public void absentStudent (String name) {
		this.students.remove(new Student (name));
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
	
	public Optional<String> callStudent () {
		Optional<Section> cur = this.getSelected();
		if (!cur.isPresent())
			return Optional.empty();
		
		Optional<String> name = 
				this.caller.callStudent(
						cur.get().getLastCalled(), 
						this.getObservableStudents(), 
						cur.get().getRandom());
		
		if (name.isPresent()) {
			this.history.push(this.sections.copy());
			
			cur.get().calledStudent(name.get());
			cur.get().setLastCalled(name.get());
			this.students.clear();
			this.students.addAll(cur.get().getStudents());
			this.dao.save(this.sections);
		}
		
		return name;
	}
	
	public Optional<String> lastCalled () {
		Optional<Section> cur = this.getSelected();
		if (cur.isPresent())
			return Optional.ofNullable(cur.get().getLastCalled());
		else
			return Optional.empty();
	}
	
	public boolean canUndo () {
		return !this.history.isEmpty();
	}
	
	public void undo () {
		if (this.history.isEmpty())
			return;
		
		this.sections = this.history.pop();
		this.dao.save(this.sections);
		
		this.students.clear();
		Optional<Section> cur = this.getSelected();
		if (cur.isPresent())
			this.students.addAll(cur.get().getStudents());
	}
	
	public void addSelectionChangedListener (ValueChangedListener<Optional<Section>> l) {
		Preconditions.checkNotNull(l);
		this.selectedListeners.add(l);
	}
	
}

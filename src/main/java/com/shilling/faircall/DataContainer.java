package com.shilling.faircall;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.Stack;
import java.util.stream.Collectors;

import com.google.common.base.Preconditions;
import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Sections;
import com.shilling.faircall.model.Student;

import javafx.beans.property.adapter.JavaBeanBooleanProperty;
import javafx.beans.property.adapter.JavaBeanBooleanPropertyBuilder;
import javafx.beans.property.adapter.JavaBeanObjectPropertyBuilder;
import javafx.beans.property.adapter.ReadOnlyJavaBeanProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;

@Singleton
public class DataContainer {
	
	private final SectionsDAO dao;
	private final Caller caller;
	
	private final Stack<Sections> history;
	
	private final ReadOnlyJavaBeanProperty <Sections> sectionsProperty;
	private Sections sections;
	private final ReadOnlyJavaBeanProperty <Optional<Section>> selectedProperty;
	private Optional<Section> selected = Optional.empty();
	private final ReadOnlyJavaBeanProperty <Optional<String>> lastCalledProperty;
	private Optional<String> lastCalled = Optional.empty();
	private final JavaBeanBooleanProperty canUndoProperty;
	private boolean canUndo = false;
	private final JavaBeanBooleanProperty canCallProperty;
	private boolean canCall = false;
	private final JavaBeanBooleanProperty randomProperty;
	private boolean random = true;
	
	private final ObservableList<String> classes;
	private final ObservableList<Student> students;
	
	private final List<Student> absent;
	
	@SuppressWarnings("unchecked")
	@Inject
	public DataContainer (Sections sections, Caller caller, SectionsDAO dao) {
		Preconditions.checkNotNull(sections);
		
		this.history = new Stack<>();
		this.sections = sections;
		this.caller = caller;
		this.dao = dao;
		this.classes = 
			FXCollections.observableArrayList(sections.getSectionNames());
		this.students =
			FXCollections.observableArrayList();
		this.absent = new ArrayList<>();

		/* Set up observable properties */
		ReadOnlyJavaBeanProperty <Sections> sectionsProperty = null;
		ReadOnlyJavaBeanProperty <Optional<Section>> selected = null;
		ReadOnlyJavaBeanProperty <Optional<String>> lastCalled = null;
		JavaBeanBooleanProperty canCall = null;
		JavaBeanBooleanProperty canUndo = null;
		JavaBeanBooleanProperty random = null;
		try {
			sectionsProperty = JavaBeanObjectPropertyBuilder.create()
					.bean(this)
					.name("sections")
					.build();
			selected = JavaBeanObjectPropertyBuilder.create()
					.bean(this)
					.name("selected")
					.build();
			lastCalled = JavaBeanObjectPropertyBuilder.create()
					.bean(this)
					.name("lastCalled")
					.build();
			canCall = JavaBeanBooleanPropertyBuilder.create()
					.bean(this)
					.name("canCall")
					.build();
			canUndo = JavaBeanBooleanPropertyBuilder.create()
					.bean(this)
					.name("canUndo")
					.build();
			random = JavaBeanBooleanPropertyBuilder.create()
					.bean(this)
					.name("random")
					.build();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		}
		
		this.sectionsProperty = sectionsProperty;
		this.selectedProperty = selected;
		this.lastCalledProperty = lastCalled;
		this.canCallProperty = canCall;
		this.canUndoProperty = canUndo;
		this.randomProperty = random;
		
		/* Bind internal properties */
		this.getSectionsProperty().addListener(new ChangeListener<Sections> () {

			@Override
			public void changed(ObservableValue<? extends Sections> observable,
					Sections oldValue, 
					Sections newValue) {
				DataContainer.this.setSelected(newValue.getSelected());
				DataContainer.this.classes.clear();
				DataContainer.this.classes.addAll(newValue.getSectionNames());
				DataContainer.this.dao.save(DataContainer.this.getSections());
			}
			
		});
		
		this.getSelectedProperty().addListener(new ChangeListener<Optional<Section>> () {

			@Override
			public void changed(ObservableValue<? extends Optional<Section>> observable, 
					Optional<Section> oldValue,
					Optional<Section> newValue) {
				if (newValue.isPresent()) {
					DataContainer.this.setLastCalled(newValue.get().getLastCalled());
					DataContainer.this.students.clear();
					DataContainer.this.students.addAll(newValue.get().getStudents());
					DataContainer.this.setRandom(newValue.get().getRandom());
				} else {
					DataContainer.this.setLastCalled(Optional.empty());
					DataContainer.this.students.clear();
				}
				
				DataContainer.this.absent.clear();
				DataContainer.this.dao.save(DataContainer.this.getSections());
			}
			
		});
		
		this.getLastCalledProperty().addListener(new ChangeListener<Optional<String>> () {

			@Override
			public void changed(ObservableValue<? extends Optional<String>> arg0, Optional<String> arg1,
					Optional<String> arg2) {
				DataContainer.this.dao.save(DataContainer.this.getSections());
			}
			
		});
		
		this.getObservableStudents().addListener(new ListChangeListener<Student> () {

			@Override
			public void onChanged(Change<? extends Student> change) {
				DataContainer.this.setCanCall(change.getList().size() > 2);
			}
			
		});
		
		this.getRandomProperty().addListener(new ChangeListener<Boolean> () {

			@Override
			public void changed(ObservableValue<? extends Boolean> observable, 
					Boolean oldValue, Boolean newValue) {
				if (DataContainer.this.getSelected().isPresent()) {
					DataContainer.this.getSelected().get().setRandom(newValue);
					DataContainer.this.dao.save(DataContainer.this.sections);
				}
			}
			
		});
		
		/* Set initial values */
		this.setSections(sections);
		this.setSelected(sections.getSelected());
		if (sections.getSelected().isPresent()) {
			this.setLastCalled(sections.getSelected().get().getLastCalled());
			this.setRandom(sections.getSelected().get().getRandom());
			this.students.addAll(sections.getSelected().get().getStudents());
		}
	}
	
	/* Property Methods */
	public void setSections (Sections sections) {
		Preconditions.checkNotNull(sections);
		this.sections = sections;
		this.getSectionsProperty().fireValueChangedEvent();
	}
	
	public Sections getSections () {
		return this.sections;
	}
	
	public ReadOnlyJavaBeanProperty <Sections> getSectionsProperty() {
		return this.sectionsProperty;
	}
	
	public void setSelected (Optional<Section> selected) {
		Preconditions.checkNotNull(selected);
		this.selected = selected;
		this.getSelectedProperty().fireValueChangedEvent();
	}
	
	public Optional<Section> getSelected () {
		return this.selected;
	}
	
	public ReadOnlyJavaBeanProperty <Optional<Section>> getSelectedProperty () {
		return this.selectedProperty;
	}
	
	public void setLastCalled (Optional<String> lastCalled) {
		Preconditions.checkNotNull(lastCalled);
		Preconditions.checkArgument(!lastCalled.isPresent() || !lastCalled.get().isEmpty());
		this.lastCalled = lastCalled;
		this.getLastCalledProperty().fireValueChangedEvent();
	}
	
	public Optional<String> getLastCalled () {
		return this.lastCalled;
	}
	
	public ReadOnlyJavaBeanProperty <Optional<String>> getLastCalledProperty() {
		return this.lastCalledProperty;
	}
	
	public void setCanUndo (boolean canUndo) {
		this.canUndo = canUndo;
		this.getCanUndoProperty().fireValueChangedEvent();
	}
	
	public boolean getCanUndo () {
		return this.canUndo;
	}
	
	public JavaBeanBooleanProperty getCanUndoProperty() {
		return this.canUndoProperty;
	}
	
	public void setCanCall (boolean canCall) {
		this.canCall = canCall;
		this.getCanCallProperty().fireValueChangedEvent();
	}
	
	public boolean getCanCall() {
		return this.canCall;
	}
	
	public JavaBeanBooleanProperty getCanCallProperty() {
		return this.canCallProperty;
	}
	
	public void setRandom (boolean canCall) {
		this.random = canCall;
		this.getRandomProperty().fireValueChangedEvent();
	}
	
	public boolean getRandom() {
		return this.random;
	}
	
	public JavaBeanBooleanProperty getRandomProperty() {
		return this.randomProperty;
	}
	
	public ObservableList<String> getObservableClasses() {
		return this.classes;
	}
	
	public ObservableList<Student> getObservableStudents() {
		return this.students;
	}
	
	
	/* Main Operations */
	
	public void createClass (String name) {
		if (this.sections.addSection(name)) {
			this.classes.add(name);
			this.select(name);
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
		this.setSelected(Optional.empty());
	}
	
	public void select (String name) {
		if (this.sections.select(name)) {
			this.setSelected(this.sections.getSelected());
		} else {
			this.setSelected(Optional.empty());
		}
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
		this.undo();
		this.absent.add(new Student (name));
	}
	
	public Optional<String> callStudent () {
		Optional<Section> cur = this.getSelected();
		if (!cur.isPresent())
			return Optional.empty();
		
		Optional<String> name = 
				this.caller.callStudent(
						cur.get().getLastCalled().isPresent() ? cur.get().getLastCalled().get() : null, 
						this.getObservableStudents().stream()
							.filter(s -> !absent.contains(s))
							.collect(Collectors.toList()), 
						cur.get().getRandom());
		
		if (name.isPresent()) {
			this.history.push(this.sections.copy());
			this.setCanUndo(true);
			
			cur.get().calledStudent(name.get());
			cur.get().setLastCalled(name.get());
			this.setLastCalled(name);
			
			this.students.clear();
			this.students.addAll(this.getSelected().get().getStudents());
		}
		
		return name;
	}
	
	public void undo () {
		if (this.getCanUndo()) {
			this.setSections(this.history.pop());
			this.setSections(sections);
			this.setSelected(sections.getSelected());
			if (sections.getSelected().isPresent()) {
				this.setLastCalled(sections.getSelected().get().getLastCalled());
				this.setRandom(sections.getSelected().get().getRandom());
				this.students.clear();
				this.students.addAll(sections.getSelected().get().getStudents());
			}
			this.setCanUndo(!this.history.isEmpty());
		}
	}
	
}

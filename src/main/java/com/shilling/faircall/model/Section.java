package com.shilling.faircall.model;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.common.base.Preconditions;

public class Section {

	@JsonProperty ("name")
	private final String name;
	@JsonProperty ("students")
	private final Map<String, Integer> students;
	@JsonProperty ("random")
	private boolean random;
	@JsonProperty ("last")
	private String lastCalled;
	
	@JsonCreator
	public Section (
			@JsonProperty ("name") String name, 
			@JsonProperty("students") Map<String, Integer> students,
			@JsonProperty("last") String lastCalled,
			@JsonProperty ("random") Boolean random) {
		Preconditions.checkNotNull(name, "Every class must have a name.");
		Preconditions.checkArgument(!name.isEmpty(), "Every class must have a name.");
		
		this.name = name;
		this.students = new HashMap<>();
		if (students != null) {
			for (String str : students.keySet()) {
				Integer called = students.get(str);
				if (called != null) {
					this.students.put(str, called.intValue());
				}
			}
		}
		
		this.random = random == null ? true : random;
		this.lastCalled = lastCalled;
	}
	
	public Section (String name) {
		this(name, null, null, null);
	}
	
	public String getName() {
		return this.name;
	}
	
	public boolean getRandom() {
		return this.random;
	}
	
	public void setRandom (boolean random) {
		this.random = random;
	}
	
	public String getLastCalled () {
		return this.lastCalled;
	}
	
	public void setLastCalled (String name) {
		this.lastCalled = name;
	}
	
	@JsonIgnore
	public Collection<String> getStudentNames() {
		return this.students.keySet();
	}
	
	@JsonIgnore Collection<Integer> getCalled() {
		return this.students.values();
	}
	
	@JsonIgnore
	public Collection<Student> getStudents() {
		Set<Student> ret = new HashSet<>();
		for (Entry<String, Integer> e : this.students.entrySet()) {
			ret.add(new Student (e.getKey(), e.getValue()));
		}
		return ret;
	}
	
	@JsonIgnore
	public Optional<Student> getStudent(String name) {
		Integer called = this.students.get(name);
		
		if (called == null)
			return Optional.empty();
		else
			return Optional.of(new Student (name, called));
	}
	
	public boolean addStudent (String student) {
		if (student == null || student.isEmpty())
			return false;
		if (this.students.containsKey(student))
			return false;
		
		return null == this.students.put(student, 0);
	}
	
	public boolean delStudent (String student) {
		return null != this.students.remove(student);
	}
	
	public void calledStudent (String name) {
		Integer called = this.students.get(name);
		if (called == null)
			return;
		
		this.students.put(name, called + 1);
	}
	
	@Override
	public boolean equals (Object o) {
		if (o instanceof Section)
			return this.name.equals(((Section) o).name);
		else
			return false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(this.name);
	}
	
	public Section copy () {
		return new Section (this.name, this.students, this.lastCalled, this.random);
	}
}


package com.shilling.faircall.model;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
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
	
	@JsonCreator
	public Section (
			@JsonProperty ("name") String name, 
			@JsonProperty("students") Map<String, Integer> students,
			@JsonProperty ("random") Boolean random) {
		Preconditions.checkNotNull(name, "Every class must have a name.");
		Preconditions.checkArgument(!name.isEmpty(), "Every class must have a name.");
		
		this.name = name;
		this.students = new HashMap<>();
		if (students != null) {
			for (String str : students.keySet()) {
				Integer called = students.get(str);
				if (called != null) {
					this.students.put(str, called);
				}
			}
		}
		
		this.random = random == null ? true : random;
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
	
	@JsonIgnore
	public Set<String> getStudentNames() {
		return this.students.keySet();
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
}


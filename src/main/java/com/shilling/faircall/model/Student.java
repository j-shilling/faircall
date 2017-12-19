package com.shilling.faircall.model;

import java.util.Objects;

import com.google.common.base.Preconditions;

public class Student {
	private final String name;
	private int called;
	
	protected Student (String name, int called) {
		Preconditions.checkNotNull(name);
		Preconditions.checkArgument(!name.isEmpty());
		Preconditions.checkArgument(called >= 0);
		
		this.name = name;
		this.called = called;
	}
	
	public Student (String name) {
		this (name, 0);
	}

	public int getCalled() {
		return called;
	}

	public void setCalled(int called) {
		this.called = called;
	}

	public String getName() {
		return name;
	}
	
	@Override
	public String toString() {
		return this.getName() + " " + this.getCalled();
	}
	
	@Override
	public boolean equals (Object obj) {
		if (obj instanceof Student)
			return this.getName().equals(((Student) obj).getName());
		return false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(this.getName());
	}
}

package com.shilling.faircall.model;

import java.util.Objects;

import com.google.common.base.Preconditions;
import com.google.common.collect.ComparisonChain;

public class Student implements Comparable<Student> {
	private final String name;
	private int called;
	
	protected Student (String name, int called) {
		Preconditions.checkArgument(null != name && !name.isEmpty());
		this.setCalled(called);
		this.name = name;
		
	}
	
	public Student (String name) {
		this (name, 0);
	}

	public int getCalled() {
		return this.called;
	}

	public void setCalled(int called) {
		Preconditions.checkArgument(called >= 0);
		this.called = called;
	}

	public String getName() {
		return this.name;
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

	@Override
	public int compareTo(Student x) {
		return ComparisonChain.start()
				.compare(this.getCalled(), x.getCalled())
				.compare(this.getName(), x.getName())
				.result();
	}
}

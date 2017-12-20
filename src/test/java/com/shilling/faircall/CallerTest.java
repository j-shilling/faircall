package com.shilling.faircall;

import java.util.Optional;
import java.util.stream.Collectors;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.shilling.faircall.guice.FaircallModule;
import com.shilling.faircall.model.Section;
import com.shilling.faircall.model.Student;

import junit.framework.TestCase;

public class CallerTest extends TestCase {
	
	private static final int N_STUDENTS = 25;
	private static final int N_CALLS = 10000;
	private static final int CHECK_INTERVAL = 10;
	
	private static final double MAX_SD = 1.0;
	
	private final Section section;
	
	public CallerTest () {
		this.section = new Section ("Test");
		
		for (int i = 0; i < N_STUDENTS; i++)
			this.section.addStudent(Integer.toString(i));
	}
	
	public void testCaller() {
		
		Injector injector = Guice.createInjector(new FaircallModule());
		Caller caller = injector.getInstance(Caller.class);
		Math math = injector.getInstance(Math.class);
		
		String lastCalled = null;
		
		for (int i = 0; i < N_CALLS; i ++) {
			Optional<String> result = caller.callStudent(lastCalled, this.section.getStudents(), true);
			assertTrue (result.isPresent());
			lastCalled = result.get();
			this.section.calledStudent(lastCalled);
			
			if (i % CHECK_INTERVAL == 0) {
				double sd = math.standardDeviation(
						this.section.getStudents()
						.stream()
						.map(Student::getCalled)
						.collect(Collectors.toList()));
				CallerTest.assertTrue("After " + i + " tests SD = " + sd, sd <= MAX_SD);
			}
		}
		
		double sd = math.standardDeviation(
				this.section.getStudents()
				.stream()
				.map(Student::getCalled)
				.collect(Collectors.toList()));
		CallerTest.assertTrue("After " + N_CALLS + " tests SD = " + sd, sd <= MAX_SD);
	}

}

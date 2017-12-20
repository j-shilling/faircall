package com.shilling.faircall;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Optional;
import java.util.Random;
import java.util.stream.Collectors;

import com.google.common.base.Preconditions;
import com.google.inject.Inject;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Student;

@Singleton
public class Caller {
	
	private final Math math;
	
	@Inject
	private Caller (Math math) {
		this.math = math;
	}
	
	private String callStudent (List<Student> fairgame) {
		Preconditions.checkArgument(fairgame.size() > 0);
		return fairgame.get(new Random().nextInt(fairgame.size())).getName();
	}
	
	private String randCall (String lastCalled, Collection<Student> available) {
		
		List<Integer> data = available.stream()
				.map(Student::getCalled)
				.collect(Collectors.toList());
		
		double min = this.math.min(data);
		double max = this.math.max(data);
		
		if (max == min) max ++;
		
		List<Student> must = new ArrayList<>();
		List<Student> can = new ArrayList<>();
		for (Student s : available) {
			if (!s.getName().equals(lastCalled)) {
				if (s.getCalled() < min)
					must.add(s);
				else if (s.getCalled() < max)
					can.add(s);
			}
		}
		
		if (must.isEmpty())
			return this.callStudent(can);
		else
			return this.callStudent(must);
	}
	
	private String evenCall (String lastCalled, Collection<Student> available) {
		List<Student> fairgame = new ArrayList<>();
		
		Integer min = null;
		for (Student s : available) {
			if (!s.getName().equals(lastCalled)) {
				if (min == null) {
					min = s.getCalled();
					fairgame.add(s);
				} else {
					if (s.getCalled() == min) {
						fairgame.add(s);
					} else if (s.getCalled() < min) {
						min = s.getCalled();
						fairgame.clear();
						fairgame.add(s);
					}
				}
			}
		}
		
		return this.callStudent(fairgame);
	}

	public Optional<String> callStudent (String lastCalled, Collection<Student> available, boolean mode) {
		Preconditions.checkNotNull(available);
		
		if (available.size() < 2)
			return Optional.empty();
		
		double sd = this.math.standardDeviation(available.stream()
				.map(Student::getCalled)
				.collect(Collectors.toList()));
		
		if (mode && (sd < 0.85))
			return Optional.of(this.randCall(lastCalled, available));
		else
			return Optional.of(this.evenCall(lastCalled, available));
	}
}

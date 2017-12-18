package com.shilling.faircall.model;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.inject.Singleton;

@Singleton
public class Sections {

	@JsonProperty ("selected")
	private String cur; /* Nullable */
	@JsonIgnore
	final private Map<String, Section> sections;
	
	@JsonCreator
	public Sections (@JsonProperty ("selected") String cur,
			@JsonProperty ("sections") Iterable<Section> sections) {
		
		if (cur != null && cur.isEmpty())
			this.cur = null;
		else
			this.cur = cur;
		
		this.sections = new HashMap<>();
		
		if (sections != null) {
			for (Section s : sections) {
				this.sections.put(s.getName(), s);
			}
		}
	}
	
	@JsonProperty ("sections")
	public Collection<Section> getSections () {
		return this.sections.values();
	}
	
	@JsonIgnore
	public Set<String> getSectionNames() {
		return this.sections.values().stream()
				.map(s -> s.getName())
				.collect(Collectors.toSet());
	}
	
	@JsonIgnore
	public Optional<Section> getSelected () {
		if (null == this.cur)
			return Optional.empty();
		
		return Optional.of(this.sections.get(cur));
	}
	
	public boolean addSection (String name) {
		if (name == null || name.isEmpty())
			return false;
		if (this.sections.containsKey(name))
			return false;
		
		this.sections.put(name, new Section (name, null));
		return true;
	}
	
	public boolean delSection (String name) {
		if (name == null || name.isEmpty())
			return false;
		
		return null != this.sections.remove(name); 
	}
	
	public boolean select (String name) {
		if (name == null || name.isEmpty() || !this.sections.containsKey(name))
			return false;
		
		this.cur = name;
		return true;
	}
	
	public void unselect () {
		this.cur = null;
	}
}

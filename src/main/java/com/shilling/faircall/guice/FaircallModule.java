package com.shilling.faircall.guice;

import com.google.inject.AbstractModule;
import com.google.inject.Provides;
import com.shilling.faircall.Caller;
import com.shilling.faircall.Math;
import com.shilling.faircall.DataContainer;
import com.shilling.faircall.StudentListView;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.model.Sections;

public class FaircallModule extends AbstractModule {

	@Override
	protected void configure() {
		this.bind(Sections.class).toProvider(SectionsDAO.class);
		this.bind(DataContainer.class);
		this.bind(Caller.class);
		this.bind(Math.class);
		this.bind(StudentListView.class);
	}
	
	@Provides
	private SectionsDAO dao () {
		return SectionsDAO.getInstance();
	}
	
}

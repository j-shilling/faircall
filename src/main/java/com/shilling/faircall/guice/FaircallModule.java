package com.shilling.faircall.guice;

import com.google.inject.AbstractModule;
import com.google.inject.Provides;
import com.shilling.faircall.dao.SectionsDAO;
import com.shilling.faircall.model.Sections;

public class FaircallModule extends AbstractModule {

	@Override
	protected void configure() {
		this.bind(Sections.class).toProvider(SectionsDAO.class);
	}
	
	@Provides
	private SectionsDAO dao () {
		return SectionsDAO.getInstance();
	}
	
}

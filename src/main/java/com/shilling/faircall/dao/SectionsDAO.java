package com.shilling.faircall.dao;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.channels.Channels;
import java.nio.channels.FileChannel;
import java.util.Optional;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.google.inject.Provider;
import com.google.inject.Singleton;
import com.shilling.faircall.model.Sections;

@Singleton
public class SectionsDAO implements Provider<Sections> {
	
	private static SectionsDAO INSTANCE = null;
	public static SectionsDAO getInstance () {
		if (null == INSTANCE)
			INSTANCE = new SectionsDAO();
		
		return INSTANCE;
	}
	
	private File getFile () throws IOException {
		File file = new File (System.getProperty("user.home"), ".faircall");
		file.createNewFile();
		return file;
	}
	
	public Sections get() {
		
		try (RandomAccessFile raf = new RandomAccessFile (this.getFile(), "rw")) {
			
			FileChannel channel = raf.getChannel();
			InputStream in = Channels.newInputStream(channel);
			
			ObjectMapper mapper = new XmlMapper();
			Optional<Sections> ret;
			
			try {
				ret = Optional.of(mapper.readValue(in, Sections.class));
			} catch (JsonParseException e) {
				ret = Optional.empty();
			}
			
			if (ret.isPresent())
				return ret.get();
			else
				return new Sections (null, null);
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return new Sections (null, null);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return new Sections (null, null);
		}
		
	}
	
	public boolean save (Sections sections) {
		
		if (sections == null)
			return false;
		
		try (RandomAccessFile raf = new RandomAccessFile (this.getFile(), "rw")) {
			
			FileChannel channel = raf.getChannel();
			OutputStream out = Channels.newOutputStream(channel);
			
			ObjectMapper mapper = new XmlMapper();
			mapper.enable(SerializationFeature.INDENT_OUTPUT);
			mapper.writeValue (out, sections);
			
			return true;
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
	}
}

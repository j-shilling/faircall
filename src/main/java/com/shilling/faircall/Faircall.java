package com.shilling.faircall;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.google.inject.Singleton;
import com.shilling.faircall.guice.FaircallModule;
import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;

@Singleton
public class Faircall extends Application
{
	
	public static final Injector injector = Guice.createInjector(new FaircallModule());
	
	private final StudentView students = injector.getInstance(StudentView.class);
	private final ContentArea content = injector.getInstance(ContentArea.class);
	private final ToolBar toolBar = injector.getInstance(ToolBar.class);
	
	private final DataContainer data = injector.getInstance(DataContainer.class);
	
    public static void main( String[] args )
    {
    	Application.launch (args);
    }

	@Override
	public void start(Stage stage) throws Exception {
		BorderPane pane = new BorderPane ();
		
		pane.setLeft(this.students);
		pane.setTop (this.toolBar);
		pane.setCenter(this.content);
		pane.setCenterShape(true);
		
		pane.addEventHandler(KeyEvent.KEY_PRESSED, ev -> {
			if (ev.getCode() == KeyCode.ENTER)
				data.callStudent();
			else if (ev.getCode() == KeyCode.BACK_SPACE)
				data.undo();
		});
		
		Scene scene = new Scene (pane, 600, 300);
		scene.getStylesheets().add("faircall.css");
		
		stage.setTitle("Faircall");
		stage.setScene (scene);
		
		stage.show();
	}
}

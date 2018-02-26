package com.shilling.faircall;

import java.io.IOException;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Button;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

public class RootControl extends GridPane {
    
    private final Stage stage;
    
    @FXML
    private Button dets;
    @FXML
    private Button full;
    
    @FXML
    private DetailsControl detailsControl;
    
    public RootControl (Stage stage) {
        this.stage = stage;
        
        FXMLLoader fxmlLoader = new FXMLLoader(this.getClass().getResource("/fxml/root.fxml"));
        fxmlLoader.setRoot(this);
        fxmlLoader.setController(this);
        
        try {
            fxmlLoader.load();
        } catch (IOException e) {
            throw new RuntimeException (e);
        }
    }
    
    public void toggleDets(ActionEvent event) {
        boolean visible = this.detailsControl.isVisible();
        
        if (visible) {
            this.detailsControl.setVisible(false);
            
            this.getColumnConstraints().get(0).setPercentWidth(0);
            this.getColumnConstraints().get(1).setPercentWidth(100);
            
            this.dets.setText("Show Details");
        } else {
            this.detailsControl.setVisible(true);
            
            this.getColumnConstraints().get(0).setPercentWidth(50);
            this.getColumnConstraints().get(1).setPercentWidth(50);
            
            this.dets.setText("Hide Details");
        }
    }
    
    public void toggleFull(ActionEvent event) {
        if (this.stage.isFullScreen()) {
            this.stage.setFullScreen(false);
            this.full.setText("Fullscreen");
        } else {
            this.stage.setFullScreen(true);
            this.full.setText("Exit");
        }
    }
}

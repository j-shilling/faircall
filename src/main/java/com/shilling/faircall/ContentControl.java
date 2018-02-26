/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.shilling.faircall;

import java.io.IOException;
import javafx.fxml.FXMLLoader;
import javafx.scene.layout.GridPane;

/**
 * FXML Controller class
 *
 * @author jake
 */
public class ContentControl extends GridPane {

    public ContentControl () {
        FXMLLoader fxmlLoader = new FXMLLoader(this.getClass().getResource("/fxml/content.fxml"));
        fxmlLoader.setRoot(this);
        fxmlLoader.setController(this);
        
        try {
            fxmlLoader.load();
        } catch (IOException e) {
            throw new RuntimeException (e);
        }
    }
    
}

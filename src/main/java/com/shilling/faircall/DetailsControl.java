/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.shilling.faircall;

import java.io.IOException;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.ComboBox;
import javafx.scene.layout.GridPane;

/**
 *
 * @author jake
 */
public class DetailsControl extends GridPane {
    
    @FXML
    private ComboBox<ProfileManager.Profile> profiles;
    
    public DetailsControl () {
        FXMLLoader fxmlLoader = new FXMLLoader(this.getClass().getResource("/fxml/details.fxml"));
        fxmlLoader.setRoot(this);
        fxmlLoader.setController(this);
        
        try {
            fxmlLoader.load();
        } catch (IOException e) {
            throw new RuntimeException (e);
        }
        
        this.profiles.setItems(ProfileManager.getInstance().getProfiles());
    }
    
}

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.shilling.faircall;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.common.base.Preconditions;
import java.io.File;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.Objects;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

/**
 *
 * @author jake
 */
public class ProfileManager {
    
    public static abstract class Profile {
        private final String name;
        
        protected Profile (String name) {
            Preconditions.checkNotNull (name);
            Preconditions.checkArgument (!name.isEmpty());
            
            this.name = name;
        }
        
        @Override
        public String toString() {
            return this.name;
        }
        
        @Override
        public boolean equals(Object o) {
            if (o instanceof Profile)
                return this.toString().equals(o.toString());
            return false;
        }
        
        @Override
        public int hashCode() {
            return Objects.hash(this.toString());
        }
    }
    
    public static class LocalProfile extends Profile {
        @JsonIgnore
        private final File file;
        
        @JsonCreator
        public LocalProfile (
                @JsonProperty ("name") String name,
                @JsonProperty ("path") String path) {
            super (name);
            
            Preconditions.checkNotNull (path);
            Preconditions.checkArgument (!path.isEmpty());
            
            this.file = Paths.get(path).toFile();
            if (!this.file.exists()) {
                try {
                    this.file.createNewFile();
                } catch (IOException e) {
                    throw new RuntimeException (e);
                }
            }
            
            Preconditions.checkArgument (this.file.canRead());
            Preconditions.checkArgument (this.file.canWrite());
        }
    
        
        @JsonProperty ("path")
        private String getPath() {
            return this.file.getAbsolutePath();
        }
    }
    
    public static class OnlineProfile extends Profile {
        private final String username;
        private final String password;
        
        @JsonCreator
        public OnlineProfile (
                @JsonProperty ("name") String name,
                @JsonProperty ("username") String username,
                @JsonProperty ("password") String password) {
            super (name);
            
            Preconditions.checkNotNull (username);
            Preconditions.checkArgument (!username.isEmpty());
            Preconditions.checkNotNull (password);
            Preconditions.checkArgument (!password.isEmpty());
            
            this.username = username;
            this.password = password;
        }
    }
    
    private static ProfileManager INSTANCE = null;
    public static ProfileManager getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new ProfileManager();
        }
        
        return INSTANCE;
    }
    
    private final ObservableList<Profile> profiles;
    
    private ProfileManager() {
        this.profiles = FXCollections.observableArrayList();
    }
    
    public ObservableList<Profile> getProfiles() {
        return this.profiles;
    }
}

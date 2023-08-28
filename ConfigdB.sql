CREATE TABLE Utenti (
  ID INT AUTO_INCREMENT,
  PRIMARY KEY (ID),
  Nome varchar(255),
  Cognome varchar(255),
  Username varchar(255) Unique,
  Password varchar(255),
  Email varchar(255) Unique,
  Data_Nascita date);

CREATE TABLE Giochi(
  ID INT AUTO_INCREMENT,
  PRIMARY KEY(ID),
  Nome varchar(255)
);

CREATE TABLE Lobby(
  ID INT AUTO_INCREMENT,
  PRIMARY KEY(ID),
  Orario Datetime,
  Nome varchar(255),
  ID_utente INT,
  FOREIGN KEY(ID_utente) REFERENCES Utenti(ID)
  
);

CREATE TABLE Account_Game (
  ID INT AUTO_INCREMENT,
  PRIMARY KEY(ID),
  ID_utente INT,
  ID_gioco INT,
  Nickname varchar(255),
  FOREIGN KEY(ID_utente) REFERENCES Utenti(ID),
  FOREIGN KEY(ID_gioco) REFERENCES Giochi(ID),
  UNIQUE(ID_utente,ID_gioco) ,
  UNIQUE(Nickname,ID_gioco)
  );
CREATE TABLE Prenotazioni(
  ID_account INT,
  ID_lobby INT,
  Nota varchar(255)DEFAULT NULL,
  Stato_Prenotazione VARCHAR(255) DEFAULT "Attesa",
  FOREIGN KEY(ID_account) REFERENCES Account_Game(ID),
  FOREIGN KEY(ID_lobby) REFERENCES Lobby(ID) ON DELETE CASCADE ,
  CONSTRAINT Utente_Lobby PRIMARY KEY (ID_account,ID_lobby)
);

INSERT INTO Giochi (Nome) Values("League of Legends"); 
INSERT INTO Giochi (Nome) Values("Teamfight Tactics"); 
INSERT INTO Giochi (Nome) Values("Counter Strike Go"); 
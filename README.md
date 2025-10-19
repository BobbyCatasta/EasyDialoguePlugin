# 🎙️ Easy Dialogue Plugin (+ CSV Importer)

A lightweight and easy-to-use **Dialogue System** for Unreal Engine 5, designed to be flexible, data-driven, and easy to integrate into any project.  
Developed as part of my **Specialization Project** at *Event Horizon School*.

---

## 🧩 Overview

The **Easy Dialogue Plugin** allows developers to quickly set up dialogue systems using external CSV files and Unreal Engine's DataTables.  
It provides a simple yet powerful workflow for importing dialogue data, managing dialogue flow, and triggering in-game events.

> 💡 Designed for readability, modularity, and learning — with commented C++ and Blueprint-compatible systems.

---

## 🚀 Main Features

- **CSV Importer**: Import dialogues directly from URLs (HTTP) into Unreal Engine DataTables.
- **Struct-based System**: Uses custom structs for flexible and decoupled dialogue data.
- **Event Triggering**: Fire events dynamically using string-based identifiers.
- **Typewriter System**: Smooth text reveal with customizable punctuation timing.
- **Blueprint & C++ Ready**: Works seamlessly in both scripting environments.
- **Error Handling & Feedback**: Real-time logs and UI notifications for CSV or saving errors.
- **Open Source & Commented Code**: Built for easy understanding and extension.

---

## 📋 CSV Formatting Rules

Each dialogue line is represented by a row in the CSV file with the following fields:

| Field | Description |
|-------|--------------|
| `Scene_ID` | Unique identifier for the dialogue sequence |
| `Speaker` | Name of the character speaking |
| `Dialogue_Text` | The actual line of dialogue |
| `Text_Speed` | Controls typewriter speed |
| `Next_Scene` | ID of the next scene in the sequence |
| `Event_Name` | Name of the event to trigger during the dialogue |

---

## 🛠️ Components

### 🧾 CSV Importer (Utility Widget)
- Handles folder management and `.uasset` overwriting.
- Provides instant UI feedback on success or error.
- Logs parsing and saving issues for debugging.

### 🎬 Dialogue Manager (Actor)
- Drag-and-drop Actor ready for in-game use.
- Single public function entry point.
- Fully implemented in C++, supporting inheritance and customization.

### 💬 Base Dialogue UI (Widget Blueprint)
- Built-in typewriter and sound feedback system.
- Automatically enables/disables during dialogue sequences.
- Uses event-driven logic.
> ⚠️ Requires the **Dialogue Manager Actor** in the level.

### 🏔️ Level Testing and Blueprint-Function Library
- Test the pre-made level for a deeper understanding of the plugin.
- A public BFL is implemented in order to make the DialogueManager acessible everywhere.
  
---

## 🔮 Future Updates

Planned improvements and new features:
- Support for importing CSV from **local files**.
- **Customizable struct system** for advanced setups.
- Further **UI decoupling** from dialogue logic.
- **Choice and response system** for branching dialogues.
- Refined `DialogueManager` as a **singleton or subsystem**.

---

## 🧑‍💻 Author

**Alessio Desidero’**  
Role: *Generalist Game Programmer*  
Project: *Specialization Project – Event Horizon School*  

---

## 📄 License

This project is open for educational and personal use.  
Feel free to explore, modify, and learn from it — attribution appreciated.

---

*Thank you for checking out the project!* 🌙

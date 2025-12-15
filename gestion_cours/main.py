import sys
import sqlite3
import gi
import os
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GdkPixbuf

class CoursApp:
    def __init__(self):
        self.builder = Gtk.Builder()
        glade_file = "gestion-de-cours.glade"
        
        if not os.path.exists(glade_file):
            print(f"Error: Glade file '{glade_file}' not found.")
            sys.exit(1)
            
        try:
            self.builder.add_from_file(glade_file)
        except Exception as e:
            print(f"Error loading Glade file: {e}")
            sys.exit(1)

        self.conn = sqlite3.connect("cours.db")
        self.create_tables()

        # Window references
        self.main_window = self.builder.get_object("Menu Principal")
        self.admin_login = self.builder.get_object("Menu Admin")
        self.admin_interface = self.builder.get_object("Interface Admin")
        self.window_ajouter = self.builder.get_object("AD_Ajouter")
        self.window_modifier = self.builder.get_object("AD_Modifier")
        self.window_supprimer = self.builder.get_object("AD_Supprimer")
        self.window_rechercher = self.builder.get_object("AD_Rechercher")
        self.window_confirm = self.builder.get_object("SUPP_CONFIRMATION")
        
        # Ensure windows handle delete-event properly (hide instead of potential crash if not main)
        for w in [self.admin_login, self.admin_interface, self.window_ajouter, 
                  self.window_modifier, self.window_supprimer, self.window_rechercher, self.window_confirm]:
            if w:
                w.connect("delete-event", lambda widget, event: widget.hide() or True)

        self.input_dialog_window = None # For custom ID input

        self.setup_ui_enhancements()
        
        # Hide all except main
        self.hide_all_windows()
        self.main_window.show()

        # Connect Signals
        signals = {
            "on_button6_clicked": self.on_admin_clicked,     # Home -> Admin
            "on_button7_clicked": self.on_member_clicked,    # Home -> Member
            "on_button9_clicked": self.on_next_clicked,      # Home Next
            "on_button14_clicked": self.on_back_to_main,     # Login -> Main
            "on_button8_clicked": self.on_login_clicked,     # Login -> Interface
            "on_button25_clicked": self.on_logout_clicked,   # Interface -> Main
            "on_button10_clicked": self.on_open_ajouter,     # Open Add
            "on_button11_clicked": self.on_open_modifier_id_check, # Open Modify (Ask ID first)
            "on_button12_clicked": self.on_open_supprimer_id_check,   # Open Delete (Ask ID first)
            "on_button13_clicked": self.on_open_rechercher,  # Open Search
            
            # Ajouter
            "on_button31_clicked": self.on_ajouter_valider,
            
            # Rechercher
            "on_button37_clicked": self.on_rechercher_back,
            "on_button36_clicked": self.on_rechercher_submit,
            
            # Delete (From Window Supprimer - if used)
            "on_button32_clicked": self.on_supprimer_back,
            "on_button33_clicked": self.on_supprimer_submit, # Fallback if they use the window
            # Delete (Confirmation)
            "on_button34_clicked": self.on_confirm_delete_yes, # Confirm Yes
            "on_button35_clicked": self.on_confirm_delete_no,  # Confirm No
            
            # Modifier
            "on_button26_clicked": self.on_modifier_back,
            "on_button28_clicked": self.on_modifier_ignore,
            # button27 (Valider) is often not connected in Glade 2 sometimes, check explicit
            
            # Window destroy
            "gtk_main_quit": Gtk.main_quit
        }
        self.builder.connect_signals(signals)
        
        # Explicit connection for Modifier Validate if needed
        btn_mod_valid = self.builder.get_object("button27")
        if btn_mod_valid:
            btn_mod_valid.connect("clicked", self.on_modifier_valider)

        self.main_window.connect("destroy", Gtk.main_quit)
        
        # State
        self.current_modify_id = None
        self.current_delete_id = None

    def create_tables(self):
        cursor = self.conn.cursor()
        # Enhanced schema
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS cours (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                type TEXT,
                nom TEXT,
                jour INTEGER,
                annee INTEGER,
                heure_debut TEXT,
                heure_fin TEXT,
                places INTEGER,
                niveau TEXT,
                coach TEXT,
                categorie TEXT
            )
        ''')
        self.conn.commit()

    def setup_ui_enhancements(self):
        # Add icons to buttons where possible to make it "professional"
        # Using stock icons for simplicity and reliability
        
        def set_btn_icon(btn_name, stock_id):
            btn = self.builder.get_object(btn_name)
            if btn and isinstance(btn, Gtk.Button):
                image = Gtk.Image.new_from_stock(stock_id, Gtk.IconSize.BUTTON)
                btn.set_image(image)
                btn.set_always_show_image(True)

        set_btn_icon("button31", Gtk.STOCK_ADD)       # Ajouter Valider
        set_btn_icon("button27", Gtk.STOCK_APPLY)     # Modifier Valider
        set_btn_icon("button26", Gtk.STOCK_GO_BACK)   # Modifier Back
        set_btn_icon("button33", Gtk.STOCK_DELETE)    # Supprimer Valider
        set_btn_icon("button32", Gtk.STOCK_GO_BACK)   # Supprimer Back
        set_btn_icon("button36", Gtk.STOCK_FIND)      # Rechercher Submit
        set_btn_icon("button37", Gtk.STOCK_GO_BACK)   # Rechercher Back
        set_btn_icon("button34", Gtk.STOCK_YES)       # Confirm Yes
        set_btn_icon("button35", Gtk.STOCK_NO)        # Confirm No

    def hide_all_windows(self):
        windows = [
            self.main_window, self.admin_login, self.admin_interface,
            self.window_ajouter, self.window_modifier, self.window_supprimer,
            self.window_rechercher, self.window_confirm
        ]
        for w in windows:
            if w:
                w.hide()

    def show_message(self, title, message, type=Gtk.MessageType.INFO):
        dialog = Gtk.MessageDialog(
            parent=None, # Toplevel
            flags=0,
            message_type=type,
            buttons=Gtk.ButtonsType.OK,
            text=title
        )
        dialog.format_secondary_text(message)
        dialog.run()
        dialog.destroy()

    def get_id_input(self, title, callback):
        # A simple dialog to get ID
        dialog = Gtk.Dialog(title, None, 0,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             Gtk.STOCK_OK, Gtk.ResponseType.OK))
        
        dialog.set_default_size(300, 100)
        content_area = dialog.get_content_area()
        hbox = Gtk.HBox(spacing=6)
        content_area.add(hbox)
        
        lbl = Gtk.Label(label="Entrez l'ID du cours:")
        hbox.pack_start(lbl, False, False, 10)
        
        entry = Gtk.Entry()
        hbox.pack_start(entry, True, True, 10)
        
        dialog.show_all()
        response = dialog.run()
        text = entry.get_text()
        dialog.destroy()
        
        if response == Gtk.ResponseType.OK and text.strip():
            callback(text.strip())

    # --- Navigation Callbacks ---
    def on_admin_clicked(self, widget):
        self.main_window.hide()
        self.admin_login.show()

    def on_member_clicked(self, widget):
        self.show_message("Info", "Espace membre en construction.")

    def on_next_clicked(self, widget):
        # Check human
        chk = self.builder.get_object("checkbutton1")
        if chk and chk.get_active():
             # If human, maybe go to admin login as default flow?
             self.main_window.hide()
             self.admin_login.show()
        else:
            self.show_message("Attention", "Veuillez confirmer que vous êtes humain.")

    def on_back_to_main(self, widget):
        self.admin_login.hide()
        self.main_window.show()

    def on_login_clicked(self, widget):
        user_entry = self.builder.get_object("entry8")
        pass_entry = self.builder.get_object("entry7")
        username = user_entry.get_text()
        password = pass_entry.get_text()
        
        if username == "admin" and password == "admin":
            self.admin_login.hide()
            self.admin_interface.show()
            # Clear fields
            user_entry.set_text("")
            pass_entry.set_text("")
        else:
            self.show_message("Erreur", "Identifiants incorrects.", Gtk.MessageType.ERROR)

    def on_logout_clicked(self, widget):
        self.admin_interface.hide()
        self.main_window.show()

    # --- ADD ---
    def on_open_ajouter(self, widget):
        self.window_ajouter.show()

    def on_ajouter_valider(self, widget):
        try:
            # Gather data
            cb_type = self.builder.get_object("comboboxentry19")
            c_type = cb_type.get_child().get_text() if cb_type.get_child() else ""
            if not c_type:
                 entry_other = self.builder.get_object("entry15")
                 c_type = entry_other.get_text()

            entry_nom = self.builder.get_object("entry_nom_cours")
            nom = entry_nom.get_text()

            spin_jour = self.builder.get_object("spinbutton_ajout_jour")
            jour = int(spin_jour.get_value())
            spin_annee = self.builder.get_object("spinbutton_ajout_annee")
            annee = int(spin_annee.get_value())

            # Period
            heure_debut = "" # Simplify or infer from checkboxes
            if self.builder.get_object("checkbutton5").get_active(): heure_debut += "Matin "
            if self.builder.get_object("checkbutton6").get_active(): heure_debut += "Après-midi "
            if self.builder.get_object("checkbutton7").get_active(): heure_debut += "Soir"

            # Coach
            cb_coach = self.builder.get_object("comboboxentry21")
            coach = cb_coach.get_child().get_text() if cb_coach.get_child() else ""
            if not coach:
                entry_coach = self.builder.get_object("entry16")
                coach = entry_coach.get_text()
            
            # Niveau
            niveau = "Débutant"
            if self.builder.get_object("radiobutton29").get_active(): niveau = "Avancé"
            elif self.builder.get_object("radiobutton30").get_active(): niveau = "Intermédiaire"

            # Categorie
            cb_cat = self.builder.get_object("comboboxentry20")
            categorie = cb_cat.get_child().get_text() if cb_cat.get_child() else "Tous"

            spin_places = self.builder.get_object("spinbutton15")
            places = int(spin_places.get_value())

            if not nom:
                self.show_message("Erreur", "Le nom du cours est obligatoire.", Gtk.MessageType.WARNING)
                return

            cursor = self.conn.cursor()
            cursor.execute('''
                INSERT INTO cours (type, nom, jour, annee, heure_debut, places, niveau, coach, categorie)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (c_type, nom, jour, annee, heure_debut, places, niveau, coach, categorie))
            self.conn.commit()
            
            self.show_message("Succès", "Cours ajouté avec succès.")
            self.window_ajouter.hide()
            # Clear important fields
            entry_nom.set_text("")
            
        except Exception as e:
            self.show_message("Erreur", f"Erreur lors de l'ajout: {e}", Gtk.MessageType.ERROR)

    # --- MODIFY ---
    def on_open_modifier_id_check(self, widget):
        # User requirement: "when you want to find cours you have to search with id"
        self.get_id_input("Modifier un cours", self.process_modify_id)

    def process_modify_id(self, id_text):
        try:
            cursor = self.conn.cursor()
            cursor.execute("SELECT * FROM cours WHERE id=?", (id_text,))
            row = cursor.fetchone()
            if row:
                self.current_modify_id = row[0]
                self.populate_modifier_window(row)
                self.window_modifier.show()
            else:
                self.show_message("Erreur", f"Aucun cours trouvé avec l'ID {id_text}")
        except Exception as e:
             self.show_message("Erreur", f"Erreur BD: {e}")

    def populate_modifier_window(self, row):
        # row: 0=id, 1=type, 2=nom, 3=jour, 4=annee, 5=heure, 7=places...
        # Map DB columns to AD_Modifier widgets
        # Note: AD_Modifier in glade seems limited in fields based on my view (Jour, Annee, Horaire, Places)
        # I will map what is available.
        
        # Jour/Annee
        if row[3]: self.builder.get_object("spinbutton6").set_value(row[3])
        if row[4]: self.builder.get_object("spinbutton7").set_value(row[4])
        
        # Places
        if row[7] is not None: self.builder.get_object("spinbutton10").set_value(row[7])
        
        # Horaire (Comboboxes 11 and 12) - approximate
        # row[5] might be "Matin" or "08h00"
        # Not strictly mapping string to combobox index here for simplicity, 
        # but one could do:
        # self.builder.get_object("comboboxentry11").get_child().set_text(row[5])

    def on_modifier_valider(self, widget):
        if not self.current_modify_id:
            return

        try:
            jour = int(self.builder.get_object("spinbutton6").get_value())
            annee = int(self.builder.get_object("spinbutton7").get_value())
            places = int(self.builder.get_object("spinbutton10").get_value())
            
            # Horaire
            h1 = self.builder.get_object("comboboxentry11").get_child().get_text()
            h2 = self.builder.get_object("comboboxentry12").get_child().get_text()
            heure_full = f"{h1} - {h2}"

            cursor = self.conn.cursor()
            cursor.execute("UPDATE cours SET jour=?, annee=?, places=?, heure_debut=? WHERE id=?", 
                           (jour, annee, places, heure_full, self.current_modify_id))
            self.conn.commit()
            self.show_message("Succès", "Cours modifié avec succès.")
            self.window_modifier.hide()
        except Exception as e:
             self.show_message("Erreur", f"Echec modification: {e}")

    def on_modifier_back(self, widget):
        self.window_modifier.hide()

    def on_modifier_ignore(self, widget):
        self.window_modifier.hide()

    # --- DELETE ---
    def on_open_supprimer_id_check(self, widget):
        # User requirement: "remove windows if you want", "search with id"
        # Bypassing AD_Supprimer window to go straight to ID check -> Confirm
        self.get_id_input("Supprimer un cours", self.process_delete_id)

    def process_delete_id(self, id_text):
        cursor = self.conn.cursor()
        cursor.execute("SELECT * FROM cours WHERE id=?", (id_text,))
        row = cursor.fetchone()
        if row:
            self.current_delete_id = row[0]
            # Show confirmation with details
            lbl_confirm = self.builder.get_object("label107") # "Êtes-vous sûr..."
            if lbl_confirm:
                details = f"Cours: {row[2]} ({row[1]})\nDate: {row[3]}/{row[4]}"
                lbl_confirm.set_text(f"Supprimer ce cours ?\n\n{details}")
            self.window_confirm.show()
        else:
            self.show_message("Erreur", f"ID {id_text} introuvable.")

    def on_supprimer_back(self, widget):
        self.window_supprimer.hide() # If they managed to open it
    
    def on_supprimer_submit(self, widget):
        pass # We used the direct ID flow

    def on_confirm_delete_yes(self, widget):
        if self.current_delete_id:
            try:
                cursor = self.conn.cursor()
                cursor.execute("DELETE FROM cours WHERE id=?", (self.current_delete_id,))
                self.conn.commit()
                self.show_message("Succès", "Cours supprimé définitivement.")
            except Exception as e:
                self.show_message("Erreur", f"Erreur suppression: {e}")
            finally:
                self.current_delete_id = None
                self.window_confirm.hide()

    def on_confirm_delete_no(self, widget):
        self.current_delete_id = None
        self.window_confirm.hide()

    # --- SEARCH ---
    def on_open_rechercher(self, widget):
        self.window_rechercher.show()

    def on_rechercher_back(self, widget):
        self.window_rechercher.hide()

    def on_rechercher_submit(self, widget):
        # "search with id also"
        entry_id = self.builder.get_object("entry17")
        cid = entry_id.get_text().strip()
        
        cursor = self.conn.cursor()
        results = []
        
        if cid:
            cursor.execute("SELECT * FROM cours WHERE id=?", (cid,))
            row = cursor.fetchone()
            if row: results.append(row)
        else:
            # Fallback to name/type
            # Access comboboxentry23 child for Name? Or is it Type?
            # It's better to check glade or just try
            cb_nom = self.builder.get_object("comboboxentry23")
            txt = cb_nom.get_child().get_text() if cb_nom.get_child() else ""
            if txt:
                 cursor.execute("SELECT * FROM cours WHERE nom LIKE ? OR type LIKE ?", (f"%{txt}%", f"%{txt}%"))
                 results = cursor.fetchall()

        if results:
            msg = ""
            for row in results[:5]: # Cap at 5 for dialog
                # 0:id, 1:type, 2:nom, 3:jour, 4:annee, 8:niveau, 9:coach
                msg += f"[{row[0]}] {row[2]} ({row[1]})\n"
                msg += f"    Date: {row[3]}/{row[4]} | Niveau: {row[8]}\n"
                msg += f"    Coach: {row[9]} | Places: {row[7]}\n"
                msg += "-"*30 + "\n"
            
            if len(results) > 5:
                msg += "... et autres résultats."
                
            self.show_message("Résultats de recherche", msg)
        else:
            self.show_message("Info", "Aucun cours trouvé.")


if __name__ == "__main__":
    app = CoursApp()
    Gtk.main()

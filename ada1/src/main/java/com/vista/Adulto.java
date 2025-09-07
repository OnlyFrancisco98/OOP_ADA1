package com.vista;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class Adulto {
    public String curp;
    public String primerNombre;
    public String segundoNombre;
    public String apellidoPaterno;
    public String apellidoMaterno;
    public Direccion direccion;
    public String telefono;
    public String email;
    public List<Dependiente> dependientes;
    public int numDependientes;

    public Adulto() {
        this.curp = "";
        this.primerNombre = "";
        this.segundoNombre = "";
        this.apellidoPaterno = "";
        this.apellidoMaterno = "";
        this.direccion = new Direccion();
        this.telefono = "";
        this.email = "";
        this.dependientes = new ArrayList<>(3);
        this.numDependientes = 0;
    }

    // Métodos de validación estáticos
    public static boolean validarFormatoCURP(String curp) {
        if (curp == null || curp.length() != 18) return false;
        
        for (int i = 0; i < 4; i++) {
            if (!Character.isLetter(curp.charAt(i))) return false;
        }
        
        for (int i = 4; i < 10; i++) {
            if (!Character.isDigit(curp.charAt(i))) return false;
        }
        
        for (int i = 10; i < 18; i++) {
            char c = curp.charAt(i);
            if (!(Character.isUpperCase(c) || Character.isDigit(c))) return false;
        }
        
        return true;
    }

    public static int calcularEdad(String curp) {
        if (curp == null || curp.length() < 10) return 0;
        
        String anioStr = curp.substring(4, 6);
        String mesStr = curp.substring(6, 8);
        String diaStr = curp.substring(8, 10);
        
        int anio = Integer.parseInt(anioStr);
        int mes = Integer.parseInt(mesStr);
        int dia = Integer.parseInt(diaStr);
        
        if (anio <= 25) {
            anio += 2000;
        } else {
            anio += 1900;
        }
        
        int edad = 2025 - anio;
        if (mes > 8 || (mes == 8 && dia > 30)) {
            edad--;
        }
        
        return edad;
    }

    public static boolean adultoExiste(List<Adulto> adultos, String curp) {
        for (Adulto adulto : adultos) {
            if (adulto.curp.equals(curp)) {
                return true;
            }
        }
        return false;
    }

    public static boolean validarTelefono(String telefono) {
        if (telefono == null || telefono.length() != 10) {
            return false;
        }
        
        for (int i = 0; i < 10; i++) {
            if (!Character.isDigit(telefono.charAt(i))) {
                return false;
            }
        }
        
        return true;
    }

    public static boolean validarDistrito(byte distrito) {
        return (distrito >= 1 && distrito <= 9);
    }

    // Función equivalente a agregarCiudadano
    public static boolean agregarCiudadano(List<Adulto> adultos, String curp, String primerNombre, 
                                         String segundoNombre, String apellidoPaterno, String apellidoMaterno,
                                         String calle, String numero, String colonia, byte distrito,
                                         String telefono, String email) {
        
        if (!validarFormatoCURP(curp)) {
            return false;
        }
        
        if (adultoExiste(adultos, curp)) {
            return false;
        }
        
        int edad = calcularEdad(curp);
        if (edad < 18) {
            return false;
        }
        
        if (!validarTelefono(telefono)) {
            return false;
        }
        
        if (!validarDistrito(distrito)) {
            return false;
        }
        
        Adulto nuevoAdulto = new Adulto();
        nuevoAdulto.curp = curp;
        nuevoAdulto.primerNombre = primerNombre;
        nuevoAdulto.segundoNombre = segundoNombre;
        nuevoAdulto.apellidoPaterno = apellidoPaterno;
        nuevoAdulto.apellidoMaterno = apellidoMaterno;
        
        nuevoAdulto.direccion.calle = calle;
        nuevoAdulto.direccion.numero = numero;
        nuevoAdulto.direccion.colonia = colonia;
        nuevoAdulto.direccion.distrito = distrito;
        
        nuevoAdulto.telefono = telefono;
        nuevoAdulto.email = email;
        
        adultos.add(nuevoAdulto);
        return true;
    }

    // Métodos de persistencia
    public static void guardarEnArchivo(List<Adulto> adultos, String nombreArchivo) {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(nombreArchivo))) {
            oos.writeObject(adultos);
            System.out.println("Datos guardados en '" + nombreArchivo + "'");
        } catch (IOException e) {
            System.out.println("Error al guardar datos: " + e.getMessage());
        }
    }

    @SuppressWarnings("unchecked")
    public static List<Adulto> cargarDesdeArchivo(String nombreArchivo) {
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(nombreArchivo))) {
            return (List<Adulto>) ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            System.out.println("No se encontraron datos previos: " + e.getMessage());
            return new ArrayList<>();
        }
    }

    // Mostrar registros
    public static void mostrarRegistros(List<Adulto> adultos) {
        if (adultos.isEmpty()) {
            System.out.println("No hay registros para mostrar.");
            return;
        }
        
        System.out.println("Total de adultos registrados: " + adultos.size());
        
        for (int i = 0; i < adultos.size(); i++) {
            Adulto adulto = adultos.get(i);
            System.out.println("\nADULTO " + (i + 1) + ":");
            System.out.println("  CURP: " + adulto.curp);
            System.out.println("  Nombre: " + adulto.primerNombre + " " + adulto.segundoNombre + " " +
                             adulto.apellidoPaterno + " " + adulto.apellidoMaterno);
            System.out.println("  Dirección: " + adulto.direccion.calle + " " + adulto.direccion.numero +
                             ", Col. " + adulto.direccion.colonia + ", Distrito " + adulto.direccion.distrito);
            System.out.println("  Teléfono: " + adulto.telefono);
            System.out.println("  Email: " + adulto.email);
            System.out.println("  Dependientes: " + adulto.numDependientes);
        }
    }

    public static void main(String[] args) {
         List<Adulto> adultos = new ArrayList<>();
        boolean resultado;
        
        System.out.println("=== SISTEMA DE REGISTRO DE CIUDADANOS ===\n");
        
        // Adulto 1 - Válido
        resultado = agregarCiudadano(adultos, 
            "GOME800101HDFTRR01", "Juan", "Carlos", "Gomez", "Perez",
            "Calle Principal", "123", "Centro", (byte)5, "5512345678", "juan@email.com");
        
        // Adulto 2 - Válido
        resultado = agregarCiudadano(adultos, 
            "RODR900215MDFMSS02", "Maria", "Elena", "Rodriguez", "Sanchez",
            "Avenida Siempre Viva", "456", "Reforma", (byte)3, "5512345679", "maria@email.com");
        
        // Adulto 3 - Válido
        resultado = agregarCiudadano(adultos, 
            "LOPA950630MDFTRR03", "Laura", "Isabel", "Lopez", "Torres",
            "Calle Secundaria", "789", "Juarez", (byte)7, "5512345680", "laura@email.com");

        mostrarRegistros(adultos);
        
        // Intentar registrar un CURP duplicado
        System.out.println("\nIntentando registrar CURP duplicado");
        resultado = agregarCiudadano(adultos, 
            "GOME800101HDFTRR01", "Juan", "Carlos", "Gomez", "Perez",
            "Otra Calle", "999", "Nueva Colonia", (byte)2, "5598765432", "juan2@email.com");

        mostrarRegistros(adultos);
        
        // Intentar registrar un menor de edad
        System.out.println("\nIntentando registrar menor de edad...");
        resultado = agregarCiudadano(adultos, 
            "MART120610HDFTRR04", "Pedro", "Luis", "Martinez", "Garcia",
            "Calle Nueva", "321", "Norte", (byte)4, "5512345681", "pedro@email.com");
        
        // Mostrar registros
        mostrarRegistros(adultos);
        
        // Intentar registrar con CURP incorrecto (debe fallar)
        System.out.println("\nIntentando registrar con CURP incorrecto...");
        resultado = agregarCiudadano(adultos, 
            "curpitoFake234", "Pepita", "Juanita", "Lopez", "Senorita",
            "Calle Falsa", "123", "Sur", (byte)6, "5512345682", "ana@email.com");
        
        // Mostrar registros finales
        System.out.println("\nMostrando registros finales:");
        mostrarRegistros(adultos);

        guardarEnArchivo(adultos, "registro_votantes.dat");
        
        System.out.println("\nPrograma terminado.");
    }
}
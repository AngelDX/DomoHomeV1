	$(document).ready(eventos);
	function eventos(){
		$(".menuitem").click(enlace_menu);
		$(".mnu a").click(enlace_menu);
		$("#btnPerfil").click(function(){
			$("#contenido").html('<div class="loader"><img src="resources/ajax-loader.gif">Cargando...</div>');
			$("#contenido").load('interface/personal_perfil.php');	
		})		
	} 
	function enlace_menu(evento){
		$("#contenido").html('<div class="loader"><img src="resources/ajax-loader.gif">Cargando...</div>');
		var pagina=$(this).attr("href");    	
      		$("#contenido").load(pagina);
	   	return false;
	}
	
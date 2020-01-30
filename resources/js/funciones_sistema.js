	$(document).ready(function(){
		$(".menuitem").click(enlace_menu);
		$(".mnu a").click(enlace_menu);
		$("#btnPerfil").click(function(){
			$("#contenido").html('<div class="loader"><img src="../resources/ajax-loader.gif">Cargando...</div>');
			$("#contenido").load('interface/personal_perfil.php');	
		})	
	});
		 
	function enlace_menu(evento){
		$("#contenido").html('<div class="loader"><img src="../resources/ajax-loader.gif">Cargando...</div>');
		var pagina=$(this).attr("href");    	
      		$("#contenido").load(pagina);
	   	return false;
	}
	
	function loading(){
	$("#contenido").html('<center><img src="../resources/ajax-loader.gif"></center>');
	}

	function fn_paginar(var_div, url){
	var div = $("#" + var_div);
	$(div).load(url);
	/*
	div.fadeOut("fast", function(){
		$(div).load(url, function(){
			$(div).fadeIn("fast");
		});
	});
	*/
	}

	function fn_mensaje_ok(data){
	  $('#msg').addClass('alert alert-success');
      $('#msg').html(data);
      $('#msg').show(300).delay(1000).hide(300);

	}
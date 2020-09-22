let dir = 1;
let speed = 1;
let rev = 0;
let volts = 0.00;
let perc = 100.00;

$(document).ready(function(){ 
    $( "#cmd_back" ).click(() => setDir(0));
    $( "#cmd_stop" ).click(() =>setDir(1));
    $( "#cmd_forward" ).click(() =>setDir(2));
    $( "#cmd_s1" ).click(() =>setSpeed(1));
    $( "#cmd_s2" ).click(() =>setSpeed(2));
    $( "#cmd_s3" ).click(() =>setSpeed(3));
    $( "#cmd_s4" ).click(() =>setSpeed(4));
    $( "#cmd_s5" ).click(() =>setSpeed(5));
    $( "#cmd_s6" ).click(() =>setSpeed(6));
    $( "#cmd_s7" ).click(() =>setSpeed(7));
    $( "#cmd_s8" ).click(() =>setSpeed(8));
    $( "#cmd_s9" ).click(() =>setSpeed(9));
    $( "#cmd_s10" ).click(() =>setSpeed(10));   
    reloadData(); 

    setInterval(() => reloadData(),500);
});


function setSpeed(speed){
    $.ajax({
        type: "POST",
        url: "/setSpeed",
        data: `${speed}`, 
        contentType: 'text/plain',
        success: function(){
            reloadData();
        },
    })
}

function setDir(dir){
    $.ajax({
        type: "POST",
        url: "/setDir",
        data: `${dir}`, 
        contentType: 'text/plain',
        success: function(){
            reloadData();
        },
    })
}

function reloadData(){
    $.ajax({
        type: "GET",
        url: "/getStatus",
        success: function(data){
            console.log(data)
            speed = data.speed;
            dir = data.dir;
            rev = data.rev;
            volts = data.volts || 0;
            perc = data.perc || 100;
            updateSpeed();
            updateDir();
            updateText();
        },
    })
}

function updateSpeed(){
    let active_name = `cmd_s${speed}`;
    let all = ['cmd_s1','cmd_s2','cmd_s3','cmd_s4','cmd_s5','cmd_s6','cmd_s7','cmd_s8','cmd_s9','cmd_s10'].filter(el => el != active_name);

    all.forEach(el => {
        $(`#${el}`).addClass("btn-outline-secondary").removeClass("btn-secondary");
    })
    $(`#${active_name}`).addClass("btn-secondary").removeClass("btn-outline-secondary")
}

function updateDir(){
    switch(dir){
        case 0:
            $(`#cmd_back`).addClass("btn-success").removeClass("btn-outline-success")
            $(`#cmd_stop`).addClass("btn-outline-danger").removeClass("btn-danger")
            $(`#cmd_forward`).addClass("btn-outline-success").removeClass("btn-success")
            break;
        case 1:
            $(`#cmd_back`).addClass("btn-outline-success").removeClass("btn-success")
            $(`#cmd_stop`).addClass("btn-danger").removeClass("btn-outline-danger")
            $(`#cmd_forward`).addClass("btn-outline-success").removeClass("btn-success")
            break;
        case 2:
            $(`#cmd_back`).addClass("btn-outline-success").removeClass("btn-success")
            $(`#cmd_stop`).addClass("btn-outline-danger").removeClass("btn-danger")
            $(`#cmd_forward`).addClass("btn-success").removeClass("btn-outline-success")
            break;
  
    }
    
}

function updateText(){
    $(`#rots`).text(` ${rev} U/min`)
    $(`#volts`).text(` ${volts} V`)
}
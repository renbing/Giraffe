
Audio.prototype.controller = new MediaController();
Audio.prototype.controller.addEventListener('canplay', function(e){this.canplay();});


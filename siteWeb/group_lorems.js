var r = new XMLHttpRequest();

r.onload = function () {
	if (r.readyState != 4 || r.status != 200) return;
	var elts = document.getElementsByClassName('introduction');

	for (var i = elts.length - 1; i >= 0; i--) {
		elts[i].textContent = r.responseText;
	}
};

r.open('GET', 'https://baconipsum.com/api/?type=meat-and-filler&paras=2&format=text');
r.send();
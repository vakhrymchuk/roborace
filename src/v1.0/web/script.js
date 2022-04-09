(function () {

    Chart.defaults.global.animation.duration = 10;

    const ctx = document.getElementById('myChart').getContext('2d');
    const myChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: ['L90', 'L60', 'L30', 'F00', 'R30', 'R60', 'R90'],
            datasets: [{
                label: 'Distance',
                data: [],
                borderWidth: 1
            }]
        },
        options: {
            duration: 10,
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero: true,
                        suggestedMax: 200
                    }
                }]
            }
        }
    });


    let socket;

    let wsReconnectTimeoutId = null;

    document.querySelector('#btn-connect').addEventListener('click', function () {
        createConnection();
    });

    function createTabs(data) {
        let tabHeaders = document.getElementById('tab-headers');
        let tabContent = document.getElementById('tab-content');
        tabHeaders.innerHTML = '';
        tabContent.innerHTML = '';

        function createHeaderDiv(name) {
            let buttonElement = document.createElement('button');
            buttonElement.className = 'tablinks';
            buttonElement.innerHTML = name;
            buttonElement.addEventListener('click', function (event) {
                openTab(event, name);
            });
            return buttonElement;
        }

        function createContentDiv(name, params) {
            let divElement = document.createElement('div');
            divElement.className = 'tabcontent';
            divElement.id = name;
            for (const param in params) {
                divElement.appendChild(createLabel(param, params[param]));
                divElement.appendChild(document.createElement('br'));
            }
            return divElement;
        }

        function createLabel(param, value) {
            function createInput() {
                let inputElement = document.createElement('input');
                inputElement.id = param;
                inputElement.type = 'number';
                inputElement.className = 'param';
                inputElement.defaultValue = value;
                return inputElement;
            }

            let labelElement = document.createElement('label');
            labelElement.appendChild(createInput());

            labelElement.innerHTML += param;
            return labelElement;
        }

        for (const name in data) {
            if (name === 't') continue;
            let params = data[name];
            // console.log(params);
            tabHeaders.appendChild(createHeaderDiv(name));
            tabContent.appendChild(createContentDiv(name, params));
        }

        let params = document.getElementsByClassName("param");
        for (let i = 0; i < params.length; i++) {
            params[i].onblur = function (event) {
                const data = {};
                data[params[i].id] = parseInt(params[i].value);
                console.log(data);
                socket && socket.send(JSON.stringify(data));
            };
        }
        document.getElementsByClassName('tablinks')[0].click();

    }

    function processData(data) {

        // document.getElementById('content').innerHTML = JSON.stringify(data);

        if (data['t'] === 'd' || data['t'] === undefined) {
            myChart.data.datasets[0].data = data['d'];
            let vMax = 12.6;
            let vMin = 9.0;
            let v = Math.min(data['v'], vMax);
            v = Math.max(v, vMin);
            let voltageProc = (v - vMin) / (vMax - vMin) * 100;
            let voltageData = data['v'].toFixed(1) + ' / ' + voltageProc.toFixed(0) + '%';
            document.getElementById('content').innerHTML =
                'strategy = ' + data['st']
                + ' angle = ' + data['a']
                + ' power = ' + data['p']
                + ' speed = ' + data['s']
                + '<br>'
                + ' fps = ' + data['f']
                + ' voltage = ' + voltageData
            ;
        } else if (data['t'] === 'p') {
            createTabs(data);
        }
        myChart.update();

    }

    const button = document.querySelector('#btn-send');
    button.addEventListener('click', function (event) {
        const data = {};
        let params = document.getElementsByClassName("param");
        for (let i = 0; i < params.length; i++) {
            data[params[i].id] = parseInt(params[i].value);
        }
        console.log(data);
        socket && socket.send(JSON.stringify(data));
    });

    let commands = document.getElementsByClassName("btn-command");
    for (const command of commands) {
        command.addEventListener('click', function (event) {
            const data = {action: command.dataset.command};
            console.log(data);
            socket && socket.send(JSON.stringify(data));
        });
    }


    createConnection();


    function createConnection() {
        const url = document.querySelector('#ip').value;
        socket = new WebSocket(url);

        function onOpen(event) {
            console.log('connected', event);
            document.querySelector('#config').setAttribute('style', 'display: none');
            document.querySelector('#control').setAttribute('style', 'display: visible');
        }

        function onClose(event) {
            console.log('disconnected', event);

            removeListeners();
            reconnect();
        }

        function onError(event) {
            console.error('error', event);

            removeListeners();
            reconnect();
        }

        function onMessage(event) {
            let data;

            try {
                data = JSON.parse(event.data);
                // console.log(data);
            } catch (error) {
                console.error('Message should have JSON format');
                return;
            }

            processData(data);
        }

        function removeListeners() {
            socket.removeEventListener('open', onOpen);
            socket.removeEventListener('close', onClose);
            socket.removeEventListener('error', onError);
            socket.removeEventListener('message', onMessage);
        }

        socket.addEventListener('open', onOpen);
        socket.addEventListener('close', onClose);
        socket.addEventListener('error', onError);
        socket.addEventListener('message', onMessage);
    }

    function reconnect() {
        if (wsReconnectTimeoutId) {
            clearTimeout(wsReconnectTimeoutId);
        }

        console.log('attempt to reconnect');

        wsReconnectTimeoutId = setTimeout(createConnection, 1000);
    }


    function openTab(evt, tabName) {
        let tabContent = document.getElementsByClassName("tabcontent");
        for (let i = 0; i < tabContent.length; i++) {
            tabContent[i].style.display = "none";
        }
        let tabLinks = document.getElementsByClassName("tablinks");
        for (let i = 0; i < tabLinks.length; i++) {
            tabLinks[i].classList.remove("active");
        }
        document.getElementById(tabName).style.display = "block";
        evt.currentTarget.classList.toggle("active");
    }

})();


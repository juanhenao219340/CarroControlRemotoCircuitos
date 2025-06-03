namespace CarroEsp8266
{
    public partial class MainPage : ContentPage
    {
        private readonly HttpClient httpClient = new();

        // IP del ESP8266 en modo AP
        private const string baseUrl = "http://192.168.4.1";
        private bool ledEncendido = false;

        public MainPage()
        {
            InitializeComponent();
        }

        private CancellationTokenSource cts;

        /// <summary>
        /// Envia un comando de manera continua mientras se mantenga presionado un botón.
        /// Utiliza un CancellationToken para poder cancelarlo al soltar el botón.
        /// </summary>
        private async Task EnviarComandoContinuo(string comando, CancellationToken token)
        {
            try
            {
                while (!token.IsCancellationRequested)
                {
                    string url = $"{baseUrl}/{comando}";
                    var response = await httpClient.GetAsync(url);
                    await Task.Delay(100); // ajusta la frecuencia del envío si es necesario
                }
            }
            catch (Exception ex)
            {
                await Dispatcher.DispatchAsync(() =>
                    DisplayAlert("Error", $"No se pudo enviar el comando: {ex.Message}", "OK"));
            }
        }

        /// <summary>
        /// Envía un comando HTTP una sola vez (para acciones instantáneas).
        /// </summary>
        private async Task EnviarComando(string comando)
        {
            try
            {
                string url = $"{baseUrl}/{comando}";
                var result = await httpClient.GetAsync(url);
            }
            catch (Exception ex)
            {
                await DisplayAlert("Error", $"No se pudo enviar el comando: {ex.Message}", "OK");
            }
        }

       // ==========================
        // BOTONES DE DIRECCIÓN
        // ==========================

        /// <summary>
        /// Al presionar el botón de avanzar, inicia el envío continuo del comando "forward".
        /// </summary
        private void OnAdelantePressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("forward");
        }

        /// <summary>
        /// Al presionar el botón de retroceder, inicia el envío continuo del comando "backward".
        /// </summary>
        private void OnAtrasPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("backward");
        }

        /// <summary>
        /// Al presionar el botón de girar a la izquierda, inicia el envío continuo del comando "left".
        /// </summary>
        private void OnIzquierdaPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("left");
        }

        /// <summary>
        /// Al presionar el botón de girar a la derecha, inicia el envío continuo del comando "right".
        /// </summary>
        private void OnDerechaPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("right");
        }

        /// <summary>
        /// Inicia el envío continuo de un comando con control de cancelación.
        /// </summary>
        private void IniciarEnvio(string comando)
        {
            cts?.Cancel(); // cancela cualquier comando anterior
            cts = new CancellationTokenSource();
            _ = EnviarComandoContinuo(comando, cts.Token); // ejecuta sin bloquear el hilo UI
        }

        /// <summary>
        /// Al soltar cualquier botón de dirección, se detiene el envío continuo y se envía el comando "stop".
        /// </summary>
        private void OnButtonReleased(object sender, EventArgs e)
        {
            AnimateReleaseButton(sender as Button);
            cts?.Cancel(); // cancela el comando continuo
            _ = EnviarComando("stop"); // manda el comando stop una vez
        }

        // =====================
        // BOTÓN DE PITAR
        // =====================

        /// <summary>
        /// Al presionar el botón de pitar, empieza a enviar "beep" continuamente.
        /// </summary>
        private void OnBeepPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            cts?.Cancel();
            cts = new CancellationTokenSource();
            _ = EnviarComandoContinuo("beep", cts.Token);
        }

        /// <summary>
        /// Al soltar el botón de pitar, detiene el envío y envía "nobeep".
        /// </summary>
        private void OnBeepReleased(object sender, EventArgs e)
        {
            AnimateReleaseButton(sender as Button);
            cts?.Cancel();
            _ = EnviarComando("nobeep");
        }

        // ==========================
        // BOTÓN DE LUZ FRONTAL
        // ==========================

        /// <summary>
        /// Alterna el estado del LED frontal (ON/OFF) y cambia el ícono del botón.
        /// </summary>
        private async void OnLedToggleClicked(object sender, EventArgs e)
        {
            ledEncendido = !ledEncendido;

            string comando = ledEncendido ? "ledon" : "ledoff";
            await EnviarComando(comando);

            // Cambia el ícono del botón
            LedButton.Text = ledEncendido ? "🔆" : "💡";
        }

        // ==========================
        // ANIMACIONES DE BOTONES
        // ==========================

        /// <summary>
        /// Efecto visual al presionar un botón (ligera reducción de tamaño).
        /// </summary>
        private async void AnimatePressButton(Button button)
        {
            await button.ScaleTo(0.9, 100, Easing.CubicOut);
        }

        /// <summary>
        /// Efecto visual al soltar un botón (regresa a tamaño normal).
        /// </summary>
        private async void AnimateReleaseButton(Button button)
        {
            await button.ScaleTo(1.0, 100, Easing.CubicIn);
        }
    }
}

namespace CarroEsp8266
{
    public partial class MainPage : ContentPage
    {
        private readonly HttpClient httpClient = new();

        // IP del ESP8266 en modo AP
        private const string baseUrl = "http://192.168.4.1";

        public MainPage()
        {
            InitializeComponent();
        }

        private CancellationTokenSource cts;

        private async Task EnviarComandoContinuo(string comando, CancellationToken token)
        {
            try
            {
                while (!token.IsCancellationRequested)
                {
                    string url = $"{baseUrl}/{comando}";
                    await httpClient.GetAsync(url);
                    await Task.Delay(100); // ajusta la frecuencia del envío si es necesario
                }
            }
            catch (Exception ex)
            {
                await Dispatcher.DispatchAsync(() =>
                    DisplayAlert("Error", $"No se pudo enviar el comando: {ex.Message}", "OK"));
            }
        }

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

        // Al presionar un botón de dirección
        private void OnAdelantePressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("forward");
        }

        private void OnAtrasPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("backward");
        }

        private void OnIzquierdaPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("left");
        }

        private void OnDerechaPressed(object sender, EventArgs e)
        {
            AnimatePressButton(sender as Button);
            IniciarEnvio("right");
        }

        private void IniciarEnvio(string comando)
        {
            cts?.Cancel(); // cancela cualquier comando anterior
            cts = new CancellationTokenSource();
            _ = EnviarComandoContinuo(comando, cts.Token); // ejecuta sin bloquear el hilo UI
        }

        // Al soltar cualquier botón
        private void OnButtonReleased(object sender, EventArgs e)
        {
            AnimateReleaseButton(sender as Button);
            cts?.Cancel(); // cancela el comando continuo
            _ = EnviarComando("stop"); // manda el comando stop una vez
        }

        private async void AnimatePressButton(Button button)
        {
            await button.ScaleTo(0.9, 100, Easing.CubicOut);
        }

        private async void AnimateReleaseButton(Button button)
        {
            await button.ScaleTo(1.0, 100, Easing.CubicIn);
        }
    }
}

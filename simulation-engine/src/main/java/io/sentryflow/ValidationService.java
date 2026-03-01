package io.sentryflow;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpServer;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * SentryFlow Digital Twin Validation Service (Java).
 * Provides HTTP endpoints for model validation used by the Python API and QA processes.
 */
public class ValidationService {

    private static final int PORT = Integer.parseInt(
        System.getenv().getOrDefault("VALIDATION_PORT", "8080"));

    public static void main(String[] args) throws IOException {
        HttpServer server = HttpServer.create(new InetSocketAddress(PORT), 0);

        server.createContext("/health", exchange -> {
            if ("GET".equals(exchange.getRequestMethod())) {
                sendJson(exchange, 200, "{\"status\":\"ok\",\"service\":\"validation\"}");
            } else {
                exchange.sendResponseHeaders(405, -1);
            }
        });

        server.createContext("/validate", exchange -> {
            if ("POST".equals(exchange.getRequestMethod())) {
                String body = new String(exchange.getRequestBody().readAllBytes(), StandardCharsets.UTF_8);
                String result = validateModel(body);
                sendJson(exchange, 200, result);
            } else {
                exchange.sendResponseHeaders(405, -1);
            }
        });

        server.setExecutor(null);
        server.start();
        System.out.println("SentryFlow Validation Service (Java) listening on port " + PORT);
    }

    private static void sendJson(HttpExchange exchange, int code, String json) throws IOException {
        byte[] bytes = json.getBytes(StandardCharsets.UTF_8);
        exchange.getResponseHeaders().set("Content-Type", "application/json");
        exchange.sendResponseHeaders(code, bytes.length);
        try (OutputStream os = exchange.getResponseBody()) {
            os.write(bytes);
        }
    }

    /**
     * Validates a digital twin model payload (e.g. JSON with modelId).
     * Returns a JSON result with checks for integration with the API.
     */
    private static String validateModel(String body) {
        boolean schemaOk = body != null && !body.isBlank();
        boolean simulationParamsOk = true; // Could validate numeric bounds, etc.

        String checks = Stream.of(
            "{\"name\":\"Schema\",\"passed\":" + schemaOk + ",\"message\":\"\"}",
            "{\"name\":\"Simulation params\",\"passed\":" + simulationParamsOk + ",\"message\":\"\"}"
        ).collect(Collectors.joining(",", "[", "]"));

        boolean passed = schemaOk && simulationParamsOk;
        return "{\"passed\":" + passed + ",\"checks\":" + checks + "}";
    }
}

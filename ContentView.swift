import SwiftUI

struct ContentView: View {
    @State private var inputText = ""
    @State private var outputText = ""

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text("Input")
                .font(.headline)

            TextEditor(text: $inputText)
                .frame(height: 120)
                .font(.system (size: 12))
                .lineSpacing(6)
                .border(Color.gray)

            Button("To Devanagari") {
                outputText = callCFunction(with: inputText)
            }

            Text("Output")
                .font(.headline)

            TextEditor(text: $outputText)
                .frame(maxWidth: .infinity, alignment: .leading)
                .font(.system (size: 16))
                .lineSpacing(8)
                .padding()
                .border(Color.blue)
        }
        .padding()
        .frame(width: 500, height: 500)
    }

    func callCFunction(with text: String) -> String {
        return text.withCString { cString in
            guard let resultPtr = convert_text(cString) else {
                return "C function returns NULL"
            }

           let result = String(cString: resultPtr)
            free_text(resultPtr)

            return result
        }
    }
}

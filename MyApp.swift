import SwiftUI

@main struct MyApp: App {
    init() {
        guard let defPath = Bundle.main.path(forResource: "define_dev_list",
                                             ofType: "tbl") else {
            print("define_dev_list.tbl is not found")
            return
        }
        
        let ok = defPath.withCString { path in
            prepare(path)
        }
        if ok != 0 {
            print("prepare is failed")
        }
        else{
            print("prepare is succeeded")
        }
    }
    
    
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

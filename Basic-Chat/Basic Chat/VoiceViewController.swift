//
//  VoiceViewController.swift
//  BasicAudio
//
//  Created by Kevin on 10/28/17.
//  Copyright © 2017 Kevin. All rights reserved.
//

import UIKit
import AVFoundation

class VoiceViewController: UIViewController, AVAudioRecorderDelegate, UITableViewDelegate, UITableViewDataSource {
    
    var recordingSession:AVAudioSession!
    var audioRecorder:AVAudioRecorder!
    var numRec:Int = 0
    var audioPlayer:AVAudioPlayer!
    var path:URL!
    var setting = [AVFormatIDKey: Int(kAudioFormatMPEG4AAC),AVSampleRateKey: 8000, AVNumberOfChannelsKey:1, AVEncoderAudioQualityKey: AVAudioQuality.min.rawValue]
    

    @IBOutlet weak var backButton: UIButton!
    @IBOutlet weak var startRecButton: UIButton!
    
    @IBOutlet weak var myTableView: UITableView!
    @IBAction func toRecord(_ sender: Any) {
        if audioRecorder == nil {
            numRec += 1
            let filename = gerDirectory().appendingPathComponent("\(numRec).m4a")
            //start recording
            
            do{
                audioRecorder = try AVAudioRecorder(url: filename, settings: setting)
                audioRecorder.delegate = self
                audioRecorder.prepareToRecord()
                audioRecorder.record()
                
                startRecButton.setTitle("Stop Recording", for: .normal)
                
            }catch{
                displayAlert(title: "Error", message: "You Sucks!")
            }
        }else{
            //stop audio record
            audioRecorder.stop()
            audioRecorder = nil
            UserDefaults.standard.set(numRec, forKey: "myNumber")
            myTableView.reloadData()
            startRecButton.setTitle("Start Recording", for: .normal)
            
        }
    }
    
    @IBAction func overrideRec(_ sender: Any) {
        numRec = 0
        UserDefaults.standard.set(numRec, forKey: "myNumber")
        myTableView.reloadData()
    }
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        
        if let number:Int = UserDefaults.standard.object(forKey: "myNumber") as? Int{
            numRec = number
        }
        
        recordingSession = AVAudioSession.sharedInstance()
        
        AVAudioSession.sharedInstance().recordPermission()
        myTableView.reloadData()
    }
    
    func gerDirectory() -> URL {
        let paths = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)
        let documentDirectory = paths[0]
        return documentDirectory
    }
    
    func displayAlert(title:String, message:String){
        let alert = UIAlertController(title: title, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "dissmis", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
    
    //Setting up table view
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return numRec
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        let path = gerDirectory().appendingPathComponent("\(indexPath.row + 1).m4a")
        cell.textLabel?.text = String("Recording # \(indexPath.row + 1) Size: \(getFileSize(sizeFileName:path)) Bytes")
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        path = gerDirectory().appendingPathComponent("\(indexPath.row + 1).m4a")
        do{
            audioPlayer = try AVAudioPlayer(contentsOf: path)
            try AVAudioSession.sharedInstance().overrideOutputAudioPort(AVAudioSessionPortOverride.speaker)
            audioPlayer.play()
        }catch{
            
        }
    }
    
    @IBAction func sendNback(_ sender: Any) {
        if (path != nil){
            performSegue(withIdentifier: "AudioBackSegue", sender: self)
        }
    }
    
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        let UartViewController = segue.destination as! UartModuleViewController
        UartViewController.sendVoiceData(voiceURL: path)
    }
    
    func getFileSize(sizeFileName:URL) -> UInt64{
        var fileSize : UInt64
        do {
            let attr = try FileManager.default.attributesOfItem(atPath: sizeFileName.path)
            fileSize = attr[FileAttributeKey.size] as! UInt64
            
            let dict = attr as NSDictionary
            fileSize = dict.fileSize()
            return fileSize
        } catch {
            print("Error: \(error)")
        }
        return 0
    }
    
}



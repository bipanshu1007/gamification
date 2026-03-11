const express = require('express');
const fs = require('fs');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static('.')); // Serve HTML files from current directory

// =================== API ENDPOINTS ===================

// Roll dice
app.get('/api/roll-dice', (req, res) => {
    const dice = Math.floor(Math.random() * 6) + 1;
    res.json({ dice: dice });
});

// Snake & Ladder move
app.post('/api/snake-ladder/move', (req, res) => {
    const { position } = req.body;
    
    const ladders = {
        3: 22, 5: 8, 11: 26, 20: 29, 36: 44, 51: 67,
        13: 42, 18: 38, 24: 56, 31: 70, 47: 85, 63: 81, 71: 91, 80: 98
    };
    
    const snakes = {
        27: 1, 21: 9, 17: 4, 19: 7, 95: 24, 99: 78
    };
    
    let newPosition = position;
    let event = 'normal';
    
    if (snakes[position]) {
        newPosition = snakes[position];
        event = 'snake';
    } else if (ladders[position]) {
        newPosition = ladders[position];
        event = 'ladder';
    }
    
    res.json({ newPosition, event });
});

// Save to leaderboard
app.post('/api/save-result', (req, res) => {
    const { game, winner } = req.body;
    const timestamp = new Date().toLocaleString();
    const entry = `${game} | Winner: ${winner} | ${timestamp}\n`;
    
    fs.appendFile('leaderboard.txt', entry, (err) => {
        if (err) {
            return res.status(500).json({ error: 'Failed to save' });
        }
        res.json({ success: true });
    });
});

// Get leaderboard
app.get('/api/leaderboard', (req, res) => {
    fs.readFile('leaderboard.txt', 'utf8', (err, data) => {
        if (err) {
            if (err.code === 'ENOENT') {
                return res.json({ data: [] });
            }
            return res.status(500).json({ error: 'Failed to read' });
        }
        
        const lines = data.trim().split('\n').filter(line => line);
        const entries = lines.map(line => {
            const parts = line.split(' | ');
            return {
                game: parts[0],
                winner: parts[1],
                timestamp: parts[2]
            };
        });
        
        res.json({ data: entries });
    });
});

// Start server
app.listen(PORT, () => {
    console.log(`🎮 Server running at http://localhost:${PORT}`);
    console.log(`📂 Open http://localhost:${PORT}/index.html in your browser`);
});
